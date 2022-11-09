#include "mbed.h"

#define MAXIMUM_BUFFER_SIZE 6

Thread thread1;
Thread thread2;

static BufferedSerial device1(D10, D9); // tx, rx  D10:tx  D9:rx
static BufferedSerial device2(D1, D0);  // tx, rx   D1:tx   D0:rx

void master_thread() {
    char buf[MAXIMUM_BUFFER_SIZE];
    char value = 0;
    int response[2] = {1,0};

    for (response[0] = 1; response[0] <= 3; response[0]++) {
        device1.write(response, 2);
        if (device1.size()) {
            device1.read(buf, 1);
            response[0] = buf[0];
            if (device1.size()) {
                device1.read(buf, 1);
                response[1] = buf[0];
                printf("Master read from slave: mode = %d val = %d\n", response[0], response[1]);
            }
        }
        ThisThread::sleep_for(1s);
    } printf("---------stop--------"); 

}

void slave_thread() {
    char mode;
    char value;
    char buf[MAXIMUM_BUFFER_SIZE];
    char msg[2];

    while (1) {
        if (device2.size()) { // with at least 1 char
            device2.read(buf, 1);
            mode = buf[0];
            if (device2.size()) { // with at least 1 char
                device2.read(buf, 1);
                value = buf[0];
                printf("Slave Read: mode=%d, value=%d\n", mode, value);

                if (device2.writable()) {
                    if (mode == 1) { // mode!=0
                        value = value + 1;
                        msg[0] = mode;
                        msg[1] = value;
                        printf("Slave Write: mode=%d, value=%d\n", mode, value);
                        device2.write(msg, 2);
                    } else if (mode == 2) {
                        value = value + 2;
                        msg[0] = mode;
                        msg[1] = value;
                        printf("Slave Write: mode=%d, value=%d\n", mode, value);
                        device2.write(msg, 2);
                    } else {
                        msg[0] = mode;
                        msg[1] = value;
                        printf("Slave Write: mode=%d, value=%d\n", mode, value);
                        device2.write(msg, 2);
                    }
                    ThisThread::sleep_for(100ms); //wait after write
                }
            }
        }
    }
}

int main() {
  // Set desired properties (9600-8-N-1).
  device1.set_baud(9600); // 兩邊使用同baud rae
  device1.set_format(
      /* bits */ 8,
      /* parity */ BufferedSerial::None,
      /* stop bit */ 1);

  // Set desired properties (9600-8-N-1).
  device2.set_baud(9600); 
  device2.set_format(
      /* bits */ 8,
      /* parity */ BufferedSerial::None,
      /* stop bit */ 1);
  thread1.start(master_thread);
  thread2.start(slave_thread);
}
