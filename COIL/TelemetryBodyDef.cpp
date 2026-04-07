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

    // Cast to uint8_t to ensure we are working with single-byte increments
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer);

    // lastPktCounter (offset 0, 1)
    body->LastPktCounter = (static_cast<uint16_t>(data[1]) << 8) | data[0];

    // currentGrade (offset 2, 3)
    body->CurrentGrade = (static_cast<uint16_t>(data[3]) << 8) | data[2];

    // hitCount (offset 4, 5)
    body->HitCount = (static_cast<uint16_t>(data[5]) << 8) | data[4];

    // heading (offset 6, 7)
    body->Heading = (static_cast<uint16_t>(data[7]) << 8) | data[6];

    // lastCmd (offset 8)
    body->LastCmd = data[8];

    // lastCmdValue (offset 9)
    body->LastCmdValue = data[9];

    // lastCmdPower (offset 10)
    body->LastCmdPower = data[10];

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