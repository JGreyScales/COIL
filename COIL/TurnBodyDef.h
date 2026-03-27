#ifndef TURNBODY_DEF_H
#define TURNBODY_DEF_H

#include <iostream>
#include <cstdint>
#include <cstring>
#include "BodyInterface.h"

class TurnBodyDef : public BodyInterface {
public:
    static const int TURN_BODY_SIZE = 3;

    uint8_t  Direction; // 1 byte
    uint16_t Duration;  // 2 bytes

    TurnBodyDef();
    virtual ~TurnBodyDef();

    int getSize() override;
    char* serialize() override;
    static TurnBodyDef* deserialize(char* data);

    void display();
};

#endif