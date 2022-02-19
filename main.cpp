
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
//#include <sstream>
#include <termios.h>
//#include <stdio.h>
#include <string.h>

/*
 * Initialise serial port for read/write
 */
int init_port() {
    int fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);
    if(fd == -1) {
        std::cerr << "Error opening /dev/ttyUSB0. Check permissions and lsof /dev/ttyUSB0." << std::endl;
        return -1;
    } else {
        struct termios tty;
        memset (&tty, 0, sizeof tty);

        if ( tcgetattr ( fd, &tty ) != 0 ) {
            std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
            return -1;
        }

        // set baud rate to 57600
        cfsetospeed (&tty, (speed_t)B57600);
        cfsetispeed (&tty, (speed_t)B57600);

        /*
        // might need this
        tty.c_cflag     &=  ~PARENB;            // Make 8n1
        tty.c_cflag     &=  ~CSTOPB;
        tty.c_cflag     &=  ~CSIZE;
        tty.c_cflag     |=  CS8;

        tty.c_cflag     &=  ~CRTSCTS;           // no flow control
        tty.c_cc[VMIN]   =  1;                  // read doesn't block
        tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
        tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
        */

        // make raw
        cfmakeraw(&tty);

        // write attributes
        tcflush( fd, TCIFLUSH );
        if ( tcsetattr ( fd, TCSANOW, &tty ) != 0) {
            std::cout << "Error " << errno << " from tcsetattr" << std::endl;
            close(fd);
            return -1;
        }
    }

    return fd;
}

/*
 * returns size of buffer
 */
int read_message(int fd, char *buf) {

    unsigned char ch = 0;
    unsigned char prevch = 0;
    int n = 0;
    int len = 0;

    do {
        prevch = ch;
        n = read(fd, &ch, 1);
        buf[len] = ch;
        len += n;
    } while(n>0 && prevch!='\r' && ch!='\n');
    return len;
}

int main(int argc, char **argv) {

    // just dump output for now
    int fd = init_port();
    if(fd != -1) {
        while(true) {
            char buf[64];
            memset(buf, '\0', sizeof buf);
            int len = read_message(fd, buf);
            for(int i=0;i<len; i++) {
                printf("%02x ", (unsigned char)buf[i]);
            }
            printf("\n");
        }
    }

    close(fd);

    return 0;
}
