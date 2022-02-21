#include "message.h"

#include <iostream>
#include <sstream>
#include <string.h>

Message::Message() {
    bufferLen = 37;
    this->buffer = new char[bufferLen];
    memset(this->buffer, '\0', bufferLen);
}

Message::Message(char* buffer, int16_t len) {
    bufferLen = len;
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
    printf("[%d]:", bufferLen);
    for(int i=0;i<bufferLen; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
}
