#pragma once

#include <cstring>

#include "HeaderDef.h"
#include "DriveBodyDef.h"
#include "TurnBodyDef.h"
#include "TelemetryBodyDef.h"
#include "AckBodyDef.h"
#include "BodyInterface.h"

#include "Utils.h"

class PktDef
{
public:
    enum CmdType
    {
        DRIVE,
        SLEEP,
        RESPONSE,
        UNDEFINED
    };

    enum Direction {
        FORWARD = 1,
        BACKWARD = 2,
        RIGHT = 3,
        LEFT = 4,
        UNDEFINED_DIRECTION = 5,
    };

    static const int HEADERSIZE = HeaderDef::HEADER_SIZE;
    static const unsigned int CRCSIZE = 1;

private:
    struct CmdPacket
    {
        HeaderDef* Header;
        BodyInterface* body;
        char CRC;
    };

    CmdPacket Packet;
    char* RawBuffer;

public:
    CmdType cmd;
    bool deleteBuffer = true;

    unsigned char countSetBits(const char* value, int size) const;

    PktDef();
    PktDef(char* rawData);
    ~PktDef();

    void SetCmd(CmdType cmd);
    void SetBodyData(BodyInterface* data);
    void SetPktCount(int value);

    CmdType GetCmd() const;
    bool GetAck() const;
    int GetLength() const;
    char* GetBodyData() const;
    int GetPktCount() const;
    CmdPacket getPacket() const;

    bool CheckCRC(char* data, int size) const;
    void CalcCRC();
    char* GenPacket();

    void Reset();
    void display();

	// I added these helper functions to make the Crow route logic cleaner, 
    bool IsTelemetryResponse() const;
    char* GetRawPacketCopy() const;
};