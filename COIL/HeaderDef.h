#ifndef HEADER_DEF_H
#define HEADER_DEF_H

#include "BodyInterface.h"

#include <iostream>
#include <cstdint>
#include <cstring>

class HeaderDef : public BodyInterface {
public:
    static const int HEADER_SIZE = 4;

    uint16_t PktCount    : 16;

    uint8_t Padding : 4;
    uint8_t Ack : 1;
    uint8_t Sleep : 1;
    uint8_t Status : 1;
    uint8_t Drive      : 1;
    
    uint8_t Length      : 8;

    HeaderDef();
    ~HeaderDef();

    int getSize() override;
    char* serialize() override;
    static HeaderDef* deserialize(char* data);

    void display();
};

#endif