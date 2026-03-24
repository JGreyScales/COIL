#pragma once

#include "HeaderDef.h"
#include "DriveBodyDef.h"
#include "TurnBodyDef.h"
#include "TelemetryBodyDef.h"

class PktDef
{
public:
    enum CmdType
    {
        DRIVE = 0,
        SLEEP = 1,
        RESPONSE = 2
    };

    static const int FORWARD = 1;
    static const int BACKWARD = 2;
    static const int RIGHT = 3;
    static const int LEFT = 4;

    static const int HEADERSIZE = 3;   // 1 byte PktCount + 1 byte flags/padding + 1 byte Length
    static const int CRCSIZE = 1;

private:
    struct CmdPacket
    {
        HeaderDef Header;
        char* Data;
        char CRC;
    };

    CmdPacket Packet;
    char* RawBuffer;

    unsigned char countSetBits(unsigned char value) const;
    unsigned char calculateCRCFromBuffer(const char* buffer, int sizeWithoutCRC) const;

public:
    PktDef();
    PktDef(char* rawData);
    ~PktDef();

    void SetCmd(CmdType cmd);
    void SetBodyData(char* data, int size);
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