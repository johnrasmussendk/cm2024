#ifndef MESSAGE_HPP_INCLUDED
#define MESSAGE_HPP_INCLUDED

#include <string>

class Message {
public:
    Message();
    Message(char* buffer, const int16_t len);
    Message(const Message &other);
    ~Message();

    void printBuf() const;
    char* getBuffer() const { return buffer; };
    int getBufferLen() const { return bufferLen; };

protected:
    char* buffer;

private:
    int bufferLen;
};

#endif // MESSAGE_HPP_INCLUDED
