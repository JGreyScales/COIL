#ifndef HEADER_DEF_H
#define HEADER_DEF_H

#include <iostream>
#include <cstdint>
#include <cstring>

// no padding between memory allocation in the compiler
// it lets us just dump the whole struct into a raw buffer already serialized
#pragma pack(push, 1)

class HeaderDef {
public:
    uint8_t PktCount    : 8;

    uint8_t Padding : 4;
    uint8_t Ack : 1;
    uint8_t Sleep : 1;
    uint8_t Status : 1;
    uint8_t Drive      : 1;
    
    uint8_t Length      : 8;

    HeaderDef();
    ~HeaderDef();

    char* serialize();
    static HeaderDef* deserialize(char* data);

    void display();
};

#pragma pack(pop)

#endif