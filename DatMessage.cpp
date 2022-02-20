#include "DatMessage.hpp"
#include <iostream>
#include <string.h>

DatMessage::DatMessage() {
}

void DatMessage::parse(char *buf)
{
    //for(int i=0;i<11; i++) {
    //    printf("%02x ", (unsigned char)buf[i]);
    //}
    //printf("\n");

    if (strncmp(buf,"CM2024 DAT", 10)!=0) {
        std::cerr << "This is not a DAT message" << std::endl;
    } else {
        // skip the "CM2024 DAT" part and use offsets matching the wiki
        char *start = buf+10;

        this->sc = start[1] | start[0] << 8;
        this->slot = start[2];
        this->chemistry = start[3];
        this->unknown1 = start[4];
        this->programState = start[5];
        this->step = start[6];
        this->minutes = start[8] | start[7] << 8;
        this->voltage = start[10] | start[9] << 8;
        this->current = start[12] | start[11] << 8;
        this->ccap = start[16] | start[15] << 8 | start[14] << 16 | start[13] << 24;
        this->dcap = start[20] | start[19] << 8 | start[18] << 16 | start[17] << 24;
        this->unknown2 = start[21];
        this->unknown3 = start[22];
        this->maxCharge = start[23];
        this->unknown4 = start[24];
        this->unknown5 = start[25];
        this->pause = start[26];
        this->capacity = start[28] | start[27] << 8;
        this->discharge = start[29];
        this->unknown6 = start[30];
        this->unknown7 = start[31];
        this->crc = start[33] | start[32] << 8;
    }
}

void DatMessage::print()
{
    std::cout << "sc:           " << (int)this->sc << std::endl;
    std::cout << "slot:         " << (int)this->slot << std::endl;
    std::cout << "chemistry:    " << (int)this->chemistry << std::endl;
    std::cout << "unknown1:     " << (int)this->unknown1 << std::endl;
    std::cout << "programState: " << (int)this->programState << std::endl;
    std::cout << "step:         " << (int)this->step << std::endl;
    std::cout << "minutes:      " << (int)this->minutes << std::endl;
    std::cout << "voltage:      " << (int)this->voltage << std::endl;
    std::cout << "current:      " << (int)this->current << std::endl;
    std::cout << "ccap:         " << this->ccap << std::endl;
    std::cout << "dcap:         " << this->dcap << std::endl;
    std::cout << "unknown2:     " << (int)this->unknown2 << std::endl;
    std::cout << "unknown3:     " << (int)this->unknown3 << std::endl;
    std::cout << "maxCharge:    " << (int)this->maxCharge << std::endl;
    std::cout << "unknown4:     " << (int)this->unknown4 << std::endl;
    std::cout << "unknown5:     " << (int)this->unknown5 << std::endl;
    std::cout << "pause:        " << (int)this->pause << std::endl;
    std::cout << "capacity:     " << (int)this->capacity << std::endl;
    std::cout << "discharge:    " << (int)this->discharge << std::endl;
    std::cout << "unknown6:     " << (int)this->unknown6 << std::endl;
    std::cout << "unknown7:     " << (int)this->unknown7 << std::endl;
    std::cout << "crc:          " << (int)this->crc << std::endl;
}


