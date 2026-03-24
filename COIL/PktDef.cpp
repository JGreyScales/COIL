#include "PktDef.h"

#include <cstring>
#include <stdexcept>
#include <cstring>

PktDef::PktDef()
{
    Packet.Header = new HeaderDef();

    Packet.body = nullptr;

    Packet.CRC = 0;
    RawBuffer = nullptr;

    cmd = CmdType::UNDEFINED;
}

PktDef::PktDef(char* rawData)
{
    this->RawBuffer = nullptr;
    if (rawData == nullptr)
    {
        this->Packet.CRC = 0;
        this->cmd = CmdType::UNDEFINED;
        return;
    }


    HeaderDef* header = HeaderDef::deserialize(rawData);
    Packet.Header = header;

    int bodySize = Packet.Header->Length - HEADERSIZE - CRCSIZE;

    if (Packet.Header->Status) {
        this->cmd = CmdType::TELE;
    } else if (Packet.Header->Ack) {
        this->cmd = CmdType::ACK;
    } else if (Packet.Header->Drive) {
        this->cmd = CmdType::DRIVE;
    } else if (Packet.Header->Sleep) {
        this->cmd = CmdType::SLEEP;
    }
    else {
        this->cmd = CmdType::UNDEFINED;
    }


    if (bodySize > 0)
    {
        char* bodyBuffer = new char[bodySize];
        std::memcpy(bodyBuffer, rawData + HEADERSIZE, bodySize);
        switch (this->cmd)
        {
        case (CmdType::ACK):
            this->Packet.body = AckBodyDef::deserialize(bodyBuffer);
            break;
        case (CmdType::DRIVE):
            this->Packet.body = DriveBodyDef::deserialize(bodyBuffer);
            break;
        case (CmdType::TELE):
            this->Packet.body = TelemetryBodyDef::deserialize(bodyBuffer);
            break;
        default:
            break;
        }
        delete[] bodyBuffer;
    }

    Packet.CRC = rawData[Packet.Header->Length - 1];
}

PktDef::~PktDef()
{
    delete this->Packet.Header;
    delete this->Packet.body;    
    if (this->RawBuffer != nullptr) {
        delete this->RawBuffer;
    }
}

void PktDef::Reset()
{
    // call deconstructor, and then rebuild a new instance ontop of old memory
    // body is not built by default, so the variable memory will be fine
    this->~PktDef();
    new (this) PktDef();
}

void PktDef::SetCmd(CmdType cmd)
{
    this->cmd = cmd;
    Packet.Header->Drive = 0;
    Packet.Header->Sleep = 0;
    Packet.Header->Status = 0;

    switch (cmd)
    {
    case DRIVE:
    case TURN:
        Packet.Header->Drive = 1;
        break;

    case SLEEP:
        Packet.Header->Sleep = 1;
        break;

    case ACK:
        Packet.Header->Status = 1;
        break;

    default:
        break;
    }
}

void PktDef::SetBodyData(BodyInterface* data)
{
    this->Packet.body = data;
}


void PktDef::SetPktCount(int value)
{
    this->Packet.Header->PktCount = static_cast<uint8_t>(value);
}
//
PktDef::CmdType PktDef::GetCmd() const
{
    return this->cmd;
}
//
bool PktDef::GetAck() const
{
    return Packet.Header->Ack != 0;
}
//
int PktDef::GetLength() const
{
    return Packet.Header->Length;
}
//
char* PktDef::GetBodyData() const
{
    return Packet.body->serialize();
}
//
int PktDef::GetPktCount() const
{
    return Packet.Header->PktCount;
}

PktDef::CmdPacket PktDef::getPacket() const
{
    return this->Packet;
}




// https://stackoverflow.com/questions/12380478/bits-counting-algorithm-brian-kernighan-in-an-integer-time-complexity
unsigned char PktDef::countSetBits(const char* value, int sizeInBytes) const
{
    if (value == nullptr) return 0;
    unsigned char totalBits = 0;

    for (size_t i = 0; i < sizeInBytes; ++i) {
        unsigned char byte = static_cast<unsigned char>(value[i]);

        while (byte != 0) {
            byte &= (byte - 1);
            totalBits++;
        }
    }

    return totalBits;
}

//
void PktDef::CalcCRC()
{

    int totalLength = Packet.Header->Length;

    delete this->RawBuffer;
    this->RawBuffer = new char[totalLength];

    if (totalLength < HEADERSIZE + CRCSIZE)
    {
        Packet.CRC = 0;
        return;
    }

    int sizeWithoutCRC = totalLength - CRCSIZE;
    int bodySize = Packet.body->getSize();


    char* serializedHeader = Packet.Header->serialize();
    std::memcpy(this->RawBuffer, serializedHeader, HEADERSIZE);
    delete[] serializedHeader;



    if (bodySize > 0 && Packet.body != nullptr)
    {
        char* serializedBody = Packet.body->serialize();
        std::memcpy(this->RawBuffer + HEADERSIZE, serializedBody, bodySize);
        delete[] serializedBody;
    }

    Packet.CRC = this->countSetBits(RawBuffer, sizeWithoutCRC);
}
//
bool PktDef::CheckCRC(char* data, int size) const
{
    if (data == nullptr || size < HEADERSIZE + CRCSIZE)
    {
        return false;
    }

    unsigned char calculated = countSetBits(data, size - CRCSIZE);
    unsigned char received = static_cast<unsigned char>(data[size - 1]);

    return calculated == received;
}
//
char* PktDef::GenPacket()
{
    int totalLength = Packet.Header->Length;

    if (totalLength < HEADERSIZE + CRCSIZE)
    {
        totalLength = HEADERSIZE + CRCSIZE;
        Packet.Header->Length = totalLength;
    }

    CalcCRC();

    delete[] this->RawBuffer;
    RawBuffer = new char[totalLength];

    char* serializedHeader = Packet.Header->serialize();
    std::memcpy(RawBuffer, serializedHeader, HEADERSIZE);
    delete[] serializedHeader;

    int bodySize = Packet.body->getSize();

    if (bodySize > 0 && Packet.body != nullptr)
    {
        char* serializedBody = Packet.body->serialize();
        std::memcpy(this->RawBuffer + HEADERSIZE, serializedBody, bodySize);
        delete[] serializedBody;
    }

    RawBuffer[totalLength - 1] = Packet.CRC;

    return RawBuffer;
}