#ifndef USBWORKER_H
#define USBWORKER_H

#include <QThread>
#include "datmessage.h"

class UsbWorker : public QThread
{
    Q_OBJECT

public:
    UsbWorker(QObject *parent = nullptr);
    ~UsbWorker();

signals:
    void sendState(const DatMessage &state);

protected:
    void run();
    void initPort();
    int readMessage();

private:
    int fd;
    char buf[64];
};

#endif // USBWORKER_H

