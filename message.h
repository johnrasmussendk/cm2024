#ifndef MESSAGE_HPP_INCLUDED
#define MESSAGE_HPP_INCLUDED

#include <string>

//#include <QMetaType>
//#include <stdint.h>

class Message {
public:
    Message();
    Message(char* buffer, int16_t len);
    Message(const Message &other);
    ~Message();

    void printBuf() const;
    char* getBuffer() const { return buffer; };
    int getBufferLen() const { return bufferLen; };

protected:

    char* buffer;
    int bufferLen;
};

//Q_DECLARE_METATYPE(Message);

#endif // MESSAGE_HPP_INCLUDED
