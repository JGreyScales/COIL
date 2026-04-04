#include "TelemetryBodyDef.h"

#include <cstring>
#include <stdexcept>

TelemetryBodyDef::TelemetryBodyDef()
    : LastPktCounter(0),
    CurrentGrade(0),
    HitCount(0),
    Heading(0),
    LastCmd(0),
    LastCmdValue(0),
    LastCmdPower(0)
{
}

TelemetryBodyDef::TelemetryBodyDef(unsigned short int lastPktCounter,
    unsigned short int currentGrade,
    unsigned short int hitCount,
    unsigned short int heading,
    unsigned char lastCmd,
    unsigned char lastCmdValue,
    unsigned char lastCmdPower)
    : LastPktCounter(lastPktCounter),
    CurrentGrade(currentGrade),
    HitCount(hitCount),
    Heading(heading),
    LastCmd(lastCmd),
    LastCmdValue(lastCmdValue),
    LastCmdPower(lastCmdPower)
{
}

int TelemetryBodyDef::getSize() {
    return TELEMETRY_BODY_SIZE;
}

char* TelemetryBodyDef::serialize()
{
    char* buffer = new char[TELEMETRY_BODY_SIZE];
    int offset = 0;

    std::memcpy(buffer + offset, &LastPktCounter, sizeof(LastPktCounter));
    offset += sizeof(LastPktCounter);

    std::memcpy(buffer + offset, &CurrentGrade, sizeof(CurrentGrade));
    offset += sizeof(CurrentGrade);

    std::memcpy(buffer + offset, &HitCount, sizeof(HitCount));
    offset += sizeof(HitCount);

    std::memcpy(buffer + offset, &Heading, sizeof(Heading));
    offset += sizeof(Heading);

    std::memcpy(buffer + offset, &LastCmd, sizeof(LastCmd));
    offset += sizeof(LastCmd);

    std::memcpy(buffer + offset, &LastCmdValue, sizeof(LastCmdValue));
    offset += sizeof(LastCmdValue);

    std::memcpy(buffer + offset, &LastCmdPower, sizeof(LastCmdPower));
    offset += sizeof(LastCmdPower);

    return buffer;
}

TelemetryBodyDef* TelemetryBodyDef::deserialize(const char* buffer)
{
    TelemetryBodyDef* body = new TelemetryBodyDef();

    if (buffer == nullptr)
    {
        return body;
    }

    int offset = 0;

    std::memcpy(&body->LastPktCounter, buffer + offset, sizeof(body->LastPktCounter));
    offset += sizeof(body->LastPktCounter);

    std::memcpy(&body->CurrentGrade, buffer + offset, sizeof(body->CurrentGrade));
    offset += sizeof(body->CurrentGrade);

    std::memcpy(&body->HitCount, buffer + offset, sizeof(body->HitCount));
    offset += sizeof(body->HitCount);

    std::memcpy(&body->Heading, buffer + offset, sizeof(body->Heading));
    offset += sizeof(body->Heading);

    std::memcpy(&body->LastCmd, buffer + offset, sizeof(body->LastCmd));
    offset += sizeof(body->LastCmd);

    std::memcpy(&body->LastCmdValue, buffer + offset, sizeof(body->LastCmdValue));
    offset += sizeof(body->LastCmdValue);

    std::memcpy(&body->LastCmdPower, buffer + offset, sizeof(body->LastCmdPower));
    offset += sizeof(body->LastCmdPower);

    return body;
}

void TelemetryBodyDef::display()
{
    std::cout << "TelemetryBodyDef" << std::endl;

    std::cout << "  LastPktCounter: " << LastPktCounter
        << "  (" << toBinary(LastPktCounter) << ")" << std::endl;

    std::cout << "  CurrentGrade : " << CurrentGrade
        << "  (" << toBinary(CurrentGrade) << ")" << std::endl;

    std::cout << "  HitCount     : " << HitCount
        << "  (" << toBinary(HitCount) << ")" << std::endl;

    std::cout << "  Heading      : " << Heading
        << "  (" << toBinary(Heading) << ")" << std::endl;

    std::cout << "  LastCmd      : " << (LastCmd)
        << "  (" << toBinary(LastCmd) << ")" << std::endl;

    std::cout << "  LastCmdValue : " << (LastCmdValue)
        << "  (" << toBinary(LastCmdValue) << ")" << std::endl;

    std::cout << "  LastCmdPower : " << (LastCmdPower)
        << "  (" << toBinary(LastCmdPower) << ")" << std::endl;
}