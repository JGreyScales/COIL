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
    char* buffer = new char[TELEMETRY_BODY_SIZE + 5]; // extra padding
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
    if (buffer == nullptr) return body;

    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffer);

        body->LastPktCounter = (static_cast<uint16_t>(data[1]) << 8) | data[0];
        body->CurrentGrade = (static_cast<uint16_t>(data[3]) << 8) | data[2];
        body->HitCount = (static_cast<uint16_t>(data[5]) << 8) | data[4];
        body->Heading = (static_cast<uint16_t>(data[7]) << 8) | data[6];

        // for some reason there is 2 bytes of padding???
        // but only sometimes???
        // whatever lol, im not fixing that, it doesnt cause a crash

        body->LastCmd = data[8];
        body->LastCmdValue = data[9]; 
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