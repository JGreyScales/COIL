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


    static const int HEADERSIZE = HeaderDef::HEADER_SIZE; // 3 bytes, breakdown is in the headerdef files
    static const int CRCSIZE = 1;

private:
    struct CmdPacket
    {
        HeaderDef* Header;
        // we opted to use a body interface, instead of a char* data as outlined in the specifications
        // in pratice this looks like
        // create a body interface object
        // set your params
        // and then just set pktDef->setBody(pointer to my body interface object)
        // later on I then call Packet.Body->Serialize() and it returns a char* of the body

        // all this logic is contained in the genPacket function ^^^

        // so behaviourally its all the same, because we also have the char* rawBuffer
        BodyInterface* body;
        char CRC;
    };

    CmdPacket Packet;

    char* RawBuffer;
public:
    CmdType cmd;

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
};