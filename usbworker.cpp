#include "usbworker.h"

#include "datmessage.h"
#include "supmessage.h"

#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <termios.h>
#include <unistd.h>


UsbWorker::UsbWorker(const char* file_name, const bool dump_output, const char format, QObject *parent)
    : QThread(parent),
      file_name(file_name),
      dump_output(dump_output),
      format(format) {
    dump_output_disabled = false;
    if (('1' <= this->format && this->format <= '8') || ('A' <= this->format && this->format <= 'B')) {
        std::cout << "Minutes Step mV mA C-cap/mAh D-cap/mAh" << std::endl;
    }
}

UsbWorker::~UsbWorker() {
    if (this->fd != -1) {
        close(this->fd);
        this->fd = -1;
    }
    if (this->fd_dump_output != -1) {
        close(this->fd_dump_output);
        this->fd_dump_output = -1;
    }

    wait();
}

void UsbWorker::readFile() {
    this->fd = open(this->file_name, O_RDWR);
    if (this->fd == -1) {
        std::cerr << "Cannot open file: " << this->file_name << std::endl;
        return;
    }
    // The first data might not be from the beginning of a message.
    bool first_data = true;
    int length;
    while (0 < (length = readMessage(first_data))) {
        if (strncmp(buf,"CM2024 SUP", 10) == 0) {
            const SupMessage supMessage = getSupMessage(length);
        } else if (strncmp(buf,"CM2024 DAT", 10)==0) {
            const DatMessage datMessage = getDatMessage(length);
        } else {
            if (!first_data) {
                std::cerr << "Unknown message content data" << std::endl;
            }
        }
        first_data = false;
    }
    if (this->fd != -1) {
        if (0 != close(this->fd)) {
            std::cerr << "Cannot close file: " << this->file_name << std::endl;
        }
        this->fd = -1;
    }
}

void UsbWorker::writeCommand(const unsigned char command, const unsigned char value) {
    unsigned char buf[11] = {command, value, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    this->writeBuffer(buf, 11);
}

void UsbWorker::toggleConnect() {
    mutex.lock();
    connect = !connect;
    mutex.unlock();
}

void UsbWorker::run() {
    // The first data might not be from the begining of a message.
    bool first_data = false;
    while(true) {
        if (connect) {
            if (fd == -1) {
                initPort();
                first_data = true;
            }
            if (fd != -1) {
                emit sendConnected(true);

                int length = readMessage(first_data);
                if (strncmp(this->buf,"CM2024 SUP", 10) == 0) {
                    const SupMessage supMessage = getSupMessage(length);
                    emit sendSup(supMessage);
                } else if (strncmp(this->buf,"CM2024 DAT", 10) == 0) {
                    const DatMessage datMessage = getDatMessage(length);
                    emit sendDat(datMessage);
                } else {
                    if (!first_data) {
                        std::cerr << "Unknown message content data" << std::endl;
                    }
                }
                first_data = false;
            }
        } else {
            if (fd != -1) {
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
    if (this->fd == -1) {
        std::cerr << "Error opening /dev/ttyUSB0. Check permissions and lsof /dev/ttyUSB0." << std::endl;
        return;
    } else {
        struct termios tty;
        memset (&tty, 0, sizeof tty);

        if (tcgetattr(this->fd, &tty) != 0) {
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

int UsbWorker::readMessage(const bool read_until_new_line) {

    unsigned char ch = 0;
    unsigned char prevch = 0;
    int n = 0;
    int length = 0;

    memset(this->buf, '\0', sizeof this->buf);

    do {
        prevch = ch;
        n = read(this->fd, &ch, 1);
        this->buf[length] = ch;
        length += n;
        if (this->file_name != nullptr && length == 0) {
            break;
        }
        // Good: This solution does not reject valid messages if the crc (offset 33-34) contains a new line.
        // Bad:  This solution does not resynchronize on a new line if data bytes are missing.
        // Option 1: Remove " || !read_until_new_line" if resynchronization of a new line is needed.
        // Option 2: Resynchronization on "CM2024 ". This string is less likely to be seen in the data than a new line is.
        // Bytes for "a new line" crc: 00 00 04 01 01 04 04 02 e4 00 ed 04 e8 00 e8 d1 00 00 2e e8 00 00 04 00 06 00 00 3c 00 00 02 02 01 0d 0a 0d 0a
        //                                   |--                    crc - part                                                        --|
        // Bytes description: counter: 0; slot: Slot 5; chemistry: #1, NiMH/CD; runningState: #1, Program running; programState: #4, Cycle; program: #4, Cycle; step: #2, Discharging;
        //                    minutes: 228; voltage: 1.261; current: 0.232; chargeCap: 537.36; dischargeCap: 594.38; voltageIndex: #4, D[1.251, 1.270]; trickleState: #0, Trickle not running;
        //                    maxCharge: #6, 3000mA; sdCardLog: #0, SD Log None; proComleteNo: 0; pause: 60; capacity: 0; discharge: #2, 250mA; maxDischarge: #2, 250mA; stepIndex: #1, 2; crc: d0a;
    } while ((prevch!='\r' || ch!='\n' || !read_until_new_line) && length<47);
    dumpRawDate(length);
    return length;
}

void UsbWorker::dumpRawDate(const int length) {
    if (this->dump_output && !this->dump_output_disabled) {
        if (this->fd_dump_output == -1) {
            this->fd_dump_output = open("./cm2024_dump.bin", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
            if (this->fd_dump_output == -1) {
                std::cerr << "Error opening 'cm2024_dump.bin' for writing." << std::endl;
                this->dump_output_disabled = true;
            }
        }
        if (this->fd_dump_output != -1) {
            const int n = write(this->fd_dump_output, buf, length);
            if (n != length) {
                std::cerr << "Cannot write dump file buffer. Wrote: " << n << ", length: " << length << ". Dump output file is closed." << std::endl;
                close(this->fd_dump_output);
                this->fd_dump_output = -1;
                this->dump_output_disabled = true;
            }
        }
    }
}

SupMessage UsbWorker::getSupMessage(const int length) {
    const SupMessage supMessage = SupMessage(buf+10, length-10);
    supMessage.print_format(this->format);
    return supMessage;
}

DatMessage UsbWorker::getDatMessage(const int length) {
    const DatMessage datMessage = DatMessage(buf+10, length-10);
    datMessage.print_format(this->format);
    const uint16_t received_crc = datMessage.getCrc();
    const uint16_t calculated_crc = this->crcModbus((unsigned char*)buf+12, length-16);
    if (received_crc != calculated_crc) {
        std::cerr << "Unexpected DAT CRC, calculated: ";
        std::cerr << std::setfill('0') << std::setw(2) << std::hex << (int) calculated_crc << std::dec;
        std::cerr << ", received: ";
        std::cerr << std::setfill('0') << std::setw(2) << std::hex << (int) received_crc << std::dec;
        std::cerr << std::endl;
    }
    return datMessage;
}

void UsbWorker::writeBuffer(const unsigned char* buffer, const size_t length) {
    if (this->fd == -1) {
        std::cerr << "Cannot write to file." << std::endl;
        return;
    }
    int n = write(this->fd, buffer, length);
    if (n != length) {
        std::cerr << "Cannot write buffer. Wrote: " << n << ", length: " << length << std::endl;
        return;
    }

    const uint16_t cm2024_crc_modbus = this->cm2024CrcModbus(buffer, length);
    unsigned char cm2024_crc_modbus_buffer[2];
    cm2024_crc_modbus_buffer[0] = cm2024_crc_modbus >> 8;
    cm2024_crc_modbus_buffer[1] = cm2024_crc_modbus & 0xFF;
    n = write(this->fd, cm2024_crc_modbus_buffer, 2);
    if (n !=2) {
        std::cerr << "Cannot write CM2024 CRC Modbus buffer. Wrote: " << n << ", length: " << 2 << std::endl;
        return;
    }
}

uint16_t UsbWorker::cm2024CrcModbus(const unsigned char* buffer, const size_t length) {
    const uint16_t crc_modbus = this->crcModbus(buffer, length);
    const uint16_t high_byte = crc_modbus >> 8;
    const uint16_t low_byte = crc_modbus & 0xFF;
    const uint16_t sum_high_byte_and_low_byte = high_byte + low_byte;
    const uint16_t carry_of_sum_high_byte_and_low_byte = sum_high_byte_and_low_byte >> 8;
    const uint16_t cm2024_crc_modbus = crc_modbus + high_byte + carry_of_sum_high_byte_and_low_byte;
    return cm2024_crc_modbus;
}

// SEE: https://github.com/lammertb/libcrc/blob/master/src/crc16.c
/*
 * uint16_t crc_modbus( const unsigned char *input_str, size_t num_bytes );
 *
 * The function crc_modbus() calculates the 16 bits Modbus CRC in one pass for
 * a byte string of which the beginning has been passed to the function. The
 * number of bytes to check is also a parameter.
 */
uint16_t UsbWorker::crcModbus(const unsigned char *buffer, const size_t length) {
    uint16_t crc;
    const unsigned char *ptr;
    size_t a;

    if ( ! this->crc_tab16_init ) {
        this->initCrc16Tab();
    }
    const uint16_t CRC_START_MODBUS = 0xFFFF;
    crc = CRC_START_MODBUS;
    ptr = buffer;
    if ( ptr != NULL ) {
        for (a=0; a<length; a++) {
            crc = (crc >> 8) ^ this->crc_tab16[ (crc ^ (uint16_t) *ptr++) & 0x00FF ];
        }
    }
    return crc;

} /* crc_modbus */

// SEE: https://github.com/lammertb/libcrc/blob/master/src/crc16.c
/*
 * void init_crc16_tab();
 *
 * For optimal performance uses the CRC16 routine a lookup table with values
 * that can be used directly in the XOR arithmetic in the algorithm. This
 * lookup table is calculated by the init_crc16_tab() routine, the first time
 * the CRC function is called.
 */
void UsbWorker::initCrc16Tab() {
    uint16_t i;
    uint16_t j;
    uint16_t crc;
    uint16_t c;

    const uint16_t CRC_POLY_16 = 0xA001;
    for (i=0; i<256; i++) {
        crc = 0;
        c   = i;
        for (j=0; j<8; j++) {
            if ( (crc ^ c) & 0x0001 ) {
                crc = ( crc >> 1 ) ^ CRC_POLY_16;
            } else {
                crc =   crc >> 1;
            }
            c = c >> 1;
        }
        this->crc_tab16[i] = crc;
    }
    this->crc_tab16_init = true;
} /* init_crc16_tab */
