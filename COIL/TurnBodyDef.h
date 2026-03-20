#ifndef TURNBODY_DEF_H
#define TURNBODY_DEF_H

#include <iostream>
#include <cstdint>
#include <cstring>

// no padding between memory allocation in the compiler
// it lets us just dump the whole struct into a raw buffer already serialized
#pragma pack(push, 1)

class TurnBodyDef {
public:
    uint8_t Direction : 8;
    uint8_t Duration : 8;
    uint8_t Power : 8;

    TurnBodyDef();
    ~TurnBodyDef();

    char* serialize();
    static TurnBodyDef* deserialize(char* data);

    void display();
};

#pragma pack(pop)

#endif