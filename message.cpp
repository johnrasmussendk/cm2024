#include "message.h"

#include <string.h>

Message::Message() {
    this->bufferLen = 37;
    this->buffer = new char[this->bufferLen];
    memset(this->buffer, '\0', this->bufferLen);
}

Message::Message(char* buffer, const int16_t len) {
    this->bufferLen = len;
    this->buffer = new char[len];
    memcpy(this->buffer, buffer, len);
}

Message::Message(const Message &other) {
    this->buffer = new char[other.getBufferLen()];
    this->bufferLen = other.getBufferLen();
    memcpy(this->buffer, other.getBuffer(), other.getBufferLen());
}

Message::~Message() {
    delete [] this->buffer;
}

void Message::printBuf() const {
    printf("MessageBuffer [%d]:", this->bufferLen);
    for(int i=0;i<this->bufferLen; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
}
