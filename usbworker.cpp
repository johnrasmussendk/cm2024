#include "usbworker.h"

#include "datmessage.h"
#include "supmessage.h"

#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>


UsbWorker::UsbWorker(QObject *parent)
    : QThread(parent)
{
}

UsbWorker::~UsbWorker()
{
    if(this->fd != -1) {
        close(this->fd);
        this->fd = -1;
    }

    wait();
}

void UsbWorker::run()
{
    while(true) {

        if(connect == true) {
            if(fd == -1) {
                initPort();
            }
            if(fd != -1) {
                emit sendConnected(true);

                int len = readMessage();

                if(strncmp(this->buf,"CM2024 SUP", 10)==0) {
                    SupMessage sup = SupMessage(buf+10, len-10);
                    emit sendSup(sup);

                } else if (strncmp(this->buf,"CM2024 DAT", 10)==0) {
                    DatMessage dat = DatMessage(buf+10, len-10);
                    emit sendDat(dat);
                } else {
                    std::cout << "tf is this" << std::endl;
                }
            }
        } else {
            if(fd != -1) {
                close(fd);
                fd = -1;
                emit sendConnected(false);
            }

            QThread::sleep(1);
        }
    }
}

void UsbWorker::initPort() {
    this->fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if(this->fd == -1) {
        std::cerr << "Error opening /dev/ttyUSB0. Check permissions and lsof /dev/ttyUSB0." << std::endl;
        return;
    } else {
        struct termios tty;
        memset (&tty, 0, sizeof tty);

        if( tcgetattr(this->fd, &tty) != 0 ) {
            std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
            return;
        }

        // set baud rate to 57600
        cfsetospeed (&tty, (speed_t)B57600);
        cfsetispeed (&tty, (speed_t)B57600);

        // might need this stuff
        /*
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
        tcflush( this->fd, TCIFLUSH );
        if ( tcsetattr ( this->fd, TCSANOW, &tty ) != 0) {
            std::cout << "Error " << errno << " from tcsetattr" << std::endl;
            return;
        }

        emit sendConnected(true);
    }
}

int UsbWorker::readMessage() {

    unsigned char ch = 0;
    unsigned char prevch = 0;
    int n = 0;
    int len = 0;

    memset(this->buf, '\0', sizeof this->buf);

    do {
        prevch = ch;
        n = read(this->fd, &ch, 1);
        this->buf[len] = ch;
        len += n;
    } while((prevch!='\r' || ch!='\n') && len<=47);
    return len;
}

void UsbWorker::toggleConnect() {
    mutex.lock();
    connect = !connect;
    mutex.unlock();
}
