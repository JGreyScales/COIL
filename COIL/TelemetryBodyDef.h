#pragma once

#include <iostream>

struct TelemetryBodyDef
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

    char* serialize() const;

    static TelemetryBodyDef deserialize(const char* buffer);

    void display() const;
};