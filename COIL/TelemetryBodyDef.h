#pragma once

#include <iostream>
#include <bitset>
#include <cstring>
#include <stdexcept>
#include <cstdint>


#include "BodyInterface.h"

template <typename T>
std::string toBinary(const T& value) {
    constexpr size_t bits = sizeof(T) * 8;
    return std::bitset<bits>(value).to_string();
}

struct TelemetryBodyDef : public BodyInterface
{
public:
    unsigned short int LastPktCounter;
    unsigned short int CurrentGrade;
    unsigned short int HitCount;
    unsigned short int Heading;
    unsigned char LastCmd;
    unsigned char LastCmdValue;
    unsigned char LastCmdPower;

    static const int TELEMETRY_BODY_SIZE = 11;

    TelemetryBodyDef();

    TelemetryBodyDef(unsigned short int lastPktCounter,
        unsigned short int currentGrade,
        unsigned short int hitCount,
        unsigned short int heading,
        unsigned char lastCmd,
        unsigned char lastCmdValue,
        unsigned char lastCmdPower);

    int getSize() override;
    
    char* serialize() override;

    static TelemetryBodyDef* deserialize(const char* buffer);

    void display();
};