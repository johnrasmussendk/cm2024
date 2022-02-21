#ifndef USBWORKER_H
#define USBWORKER_H

#include "datmessage.h"
#include "supmessage.h"

#include <QMutex>
#include <QThread>


class UsbWorker : public QThread
{
    Q_OBJECT

public:
    UsbWorker(QObject *parent = nullptr);
    ~UsbWorker();

public slots:
    void toggleConnect();

signals:
    void sendDat(const DatMessage &state);
    void sendSup(const SupMessage &state);
    void sendConnected(bool connected);

protected:
    void run();
    void initPort();
    int readMessage();

private:
    int fd = -1;
    char buf[64];
    QMutex mutex;
    bool connect = false;
};

#endif // USBWORKER_H

