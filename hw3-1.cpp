#include "mbed.h"

Thread thread_master;
Thread thread_slave;

//master

SPI spi(D11, D12, D13); // mosi, miso, sclk
DigitalOut cs(D9);

SPISlave device(PD_4, PD_3, PD_1, PD_0); //mosi, miso, sclk, cs; PMOD pins

DigitalOut led(LED3);

int slave() {
  device.format(8, 3);
  device.frequency(1000000);
  // device.reply(0x00); // Prime SPI with first reply
  while (1) {
    if (device.receive()) {
      int mode = device.read(); // Read mode from master
      printf("mode = %0x\n", mode);
      if (mode == 0x01) {     // mode 01
        int v;
        v=0x01;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Confirmation to master
        v = device.read(); // Read another byte from master
        printf("Read from master: v = %d\n", v);
        v = v + 1;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Read again to allow master read back
      } else if(mode == 0x02){ // mode 02
        int v;
        v=0x02;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Confirmation to master
        v = device.read(); // Read another byte from master
        printf("Read from master: v = %d\n", v);
        v = v + 2;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Read again to allow master read back
      }
      else { //Undefined mode
        printf("Default reply to master: 0x00\n");
        device.reply(0x00); // Reply default value
        int v;
        v=0x00;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Confirmation to master
        v = device.read(); // Read another byte from master
        printf("Read from master: v = %d\n", v);
        v = v + 0;
        device.reply(v);   // Make this the next reply
        v = device.read(); // Read again to allow master read back
      };
    }
  }
}

void master()
{
   int value = 0;
   // Setup the spi for 8 bit data, high steady state clock,
   // second edge capture, with a 1MHz clock rate
   spi.format(8, 3);
   spi.frequency(1000000);
    for (int j = 0; j < 3; j++) {
        cs = 1;
        printf("---------mode %d --------\n", j);
        int response;
        printf("Send handshaking codes.\n");
        cs = 0;
        spi.write(j); //Send ID
        ThisThread::sleep_for(100ms); //Wait for debug print
        response = spi.write(j);
        ThisThread::sleep_for(100ms); //Wait for debug print
        printf("First response from slave = %d\n", response);
        cs = 1;                       // Deselect the device
        cs = 0;
        printf("Send value = %d\n", value);

        spi.write(value); //Send number to slave
        ThisThread::sleep_for(100ms); //Wait for debug print
        response = spi.write(value); //Read slave reply
        ThisThread::sleep_for(100ms); //Wait for debug print
        printf("Second response from slave = %d\n\n", response);
        cs = 1; // Deselect the device
        value += 1;
    } printf("---------stop--------");
}

int main()
{
   thread_slave.start(slave);
   thread_master.start(master);
}