#ifndef HEADER_DEF_H
#define HEADER_DEF_H

#include "BodyInterface.h"

#include <iostream>
#include <cstdint>
#include <cstring>

class HeaderDef : public BodyInterface {
public:
    static const int HEADER_SIZE = 4;

    unsigned short int PktCount;

    uint8_t Padding; 
    uint8_t Ack;     
    uint8_t Sleep;
    uint8_t Status;
    uint8_t Drive;

    uint8_t Length;

    HeaderDef();
    ~HeaderDef();

    int getSize() override;
    char* serialize() override;
    static HeaderDef* deserialize(char* data);

    void display();
};

#endif