#pragma once

#include <cstring>


#include "HeaderDef.h"
#include "DriveBodyDef.h"
#include "TurnBodyDef.h"
#include "TelemetryBodyDef.h"
#include "AckBodyDef.h"
#include "BodyInterface.h"

class PktDef
{
public:
    enum CmdType
    {
        DRIVE,
        SLEEP,
        ACK,
        TELE,
        TURN,
        UNDEFINED
    };

    enum Direction {
        FORWARD = 1,
        BACKWARD = 2,
        RIGHT = 3,
        LEFT = 4,
        UNDEFINED_DIRECTION = 5,
    };


    static const int HEADERSIZE = HeaderDef::HEADER_SIZE; // 3 bytes, breakdown is in the headerdef files
    static const int CRCSIZE = 1;

private:
    struct CmdPacket
    {
        HeaderDef* Header;
        BodyInterface* body;
        char CRC;
    };

    CmdPacket Packet;
    CmdType cmd;

    char* RawBuffer;
public:
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

    bool CheckCRC(char* data, int size) const;
    void CalcCRC();
    char* GenPacket();

    void Reset();
};