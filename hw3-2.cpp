#include "mbed.h"
#include <cstdio>

I2CSlave i2c_slave(D14, D15);
I2C m_i2c(A4, A5);
char m_addr = 0xA0;
int value = 0;
int mode = 0;
char buf[10];
char msg[2]={0,0};
Thread thread_master;
Thread thread_slave;

int slave()
{
    i2c_slave.address(0xA0);
  while (1) {
    int receive_code = i2c_slave.receive();
    switch (receive_code) {
    case I2CSlave::ReadAddressed:
      if (mode==1) { // mode!=0
        value = value + 1;
        msg[0] = mode;
        msg[1] = value;
        printf("Slave Write: mode=%d, value=%d\n", mode, value);
        i2c_slave.write(msg, 2);
      } else if (mode==2) {
        value = value + 2;
        msg[0] = mode;
        msg[1] = value;
        printf("Slave Write: mode=%d, value=%d\n", mode, value);
        i2c_slave.write(msg, 2);
      } else {
        msg[0] = mode;
        msg[1] = value;
        printf("Slave Write: mode=%d, value=%d\n", mode, value);
        i2c_slave.write(msg, 2);
      }
      break;
    case I2CSlave::WriteGeneral:
      i2c_slave.read(buf, 2);
      mode = buf[0];
      value = buf[1];
      printf("Slave Read General: mode=%d, value=%d\n", mode, value);
      break;
    case I2CSlave::WriteAddressed:
      i2c_slave.read(buf, 2);
      mode = buf[0];
      value = buf[1];
      printf("Slave Read Addressed: mode=%d, value=%d\n", mode, value);
      break;
    }
    for (int i = 0; i < 10; i++) {
      buf[i] = 0; // Clear buffer
    }
  }
}

void master()
{
    char write_buf[2] = {0,0};
    for(int i = 0; i < 3; i++) {
        write_buf[0] = i;
        write_buf[1]++;
        mode = write_buf[0];
        value = write_buf[1];
        m_i2c.write(m_addr, write_buf, 2);
        printf("-----mode %d-----\n", i);
        printf("Write mode: %d value: %d\n", write_buf[0], write_buf[1]);
        ThisThread::sleep_for(1s);
        m_i2c.read(m_addr, msg, 2);
        printf("Read mode: %d value: %d\n", msg[0], msg[1]);
        ThisThread::sleep_for(1s);
    }
}

int main()
{
    thread_master.start(master);
    thread_slave.start(slave);
}