#ifndef USBWORKER_H
#define USBWORKER_H

#include "datmessage.h"
#include "supmessage.h"

#include <QMutex>
#include <QThread>


class UsbWorker : public QThread {
    Q_OBJECT

public:
    UsbWorker(const char* file_name, const bool dump_output, const char format, QObject *parent = nullptr);
    ~UsbWorker();
    void readFile();
    void writeCommand(const unsigned char command, const unsigned char value);

public slots:
    void toggleConnect();

signals:
    void sendDat(const DatMessage &state);
    void sendSup(const SupMessage &state);
    void sendConnected(bool connected);

protected:
    void run();

private:
    const char* file_name;
    const bool dump_output;
    const char format;
    int fd = -1;
    bool dump_output_disabled;
    int fd_dump_output = -1;
    char buf[64];
    QMutex mutex;
    bool connect = false;
    void initPort();
    int readMessage(const bool read_until_new_line);
    void dumpRawDate(const int length);
    SupMessage getSupMessage(const int length);
    DatMessage getDatMessage(const int length);
    void writeBuffer(const unsigned char* buffer, const size_t length);
    uint16_t cm2024CrcModbus(const unsigned char* buffer, const size_t length);
    // SEE, down: https://github.com/lammertb/libcrc/blob/master/src/crc16.c
    bool crc_tab16_init = false;
    uint16_t crc_tab16[256];
    uint16_t crcModbus(const unsigned char *input_str, const size_t length);
    void initCrc16Tab();
    // SEE, up: https://github.com/lammertb/libcrc/blob/master/src/crc16.c
};

#endif // USBWORKER_H
