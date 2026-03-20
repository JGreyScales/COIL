#ifndef TURNBODY_DEF_H
#define TURNBODY_DEF_H

#include <iostream>
#include <cstdint>
#include <cstring>

// no padding between memory allocation in the compiler
// it lets us just dump the whole struct into a raw buffer already serialized
#pragma pack(push, 1)

class TurnBodyDef
{
public:
    uint8_t Direction : 8;
    uint16_t Duration : 16;

    TurnBodyDef();

    char* serialize();
    static TurnBodyDef* deserialize(char* data);
    void display();

    ~TurnBodyDef();
};
#pragma pack(pop)

#endif