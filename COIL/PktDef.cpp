#include "PktDef.h"

#include <cstring>
#include <stdexcept>

PktDef::PktDef()
{
    Packet.Header.PktCount = 0;
    Packet.Header.Drive = 0;
    Packet.Header.Status = 0;
    Packet.Header.Sleep = 0;
    Packet.Header.Ack = 0;
    Packet.Header.Padding = 0;
    Packet.Header.Length = 0;

    Packet.Data = nullptr;
    Packet.CRC = 0;
    RawBuffer = nullptr;
}

PktDef::PktDef(char* rawData)
{
    Packet.Data = nullptr;
    RawBuffer = nullptr;
    Packet.CRC = 0;

    if (rawData == nullptr)
    {
        Packet.Header.PktCount = 0;
        Packet.Header.Drive = 0;
        Packet.Header.Status = 0;
        Packet.Header.Sleep = 0;
        Packet.Header.Ack = 0;
        Packet.Header.Padding = 0;
        Packet.Header.Length = 0;
        return;
    }

    HeaderDef* header = HeaderDef::deserialize(rawData);

    Packet.Header = *header;
    delete header;

    int bodySize = Packet.Header.Length - HEADERSIZE - CRCSIZE;

    if (bodySize > 0)
    {
        Packet.Data = new char[bodySize];
        std::memcpy(Packet.Data, rawData + HEADERSIZE, bodySize);
    }
    else
    {
        Packet.Data = nullptr;
    }

    Packet.CRC = rawData[Packet.Header.Length - 1];
}

PktDef::~PktDef()
{
    delete[] Packet.Data;
    Packet.Data = nullptr;

    delete[] RawBuffer;
    RawBuffer = nullptr;
}

void PktDef::Reset()
{
    delete[] Packet.Data;
    Packet.Data = nullptr;

    delete[] RawBuffer;
    RawBuffer = nullptr;

    Packet.Header.PktCount = 0;
    Packet.Header.Drive = 0;
    Packet.Header.Status = 0;
    Packet.Header.Sleep = 0;
    Packet.Header.Ack = 0;
    Packet.Header.Padding = 0;
    Packet.Header.Length = 0;

    Packet.CRC = 0;
}

void PktDef::SetCmd(CmdType cmd)
{
    Packet.Header.Drive = 0;
    Packet.Header.Sleep = 0;
    Packet.Header.Status = 0;

    switch (cmd)
    {
    case DRIVE:
        Packet.Header.Drive = 1;
        break;

    case SLEEP:
        Packet.Header.Sleep = 1;
        break;

    case RESPONSE:
        Packet.Header.Status = 1;
        break;

    default:
        break;
    }
}

void PktDef::SetBodyData(char* data, int size)
{
    delete[] Packet.Data;
    Packet.Data = nullptr;

    if (data != nullptr && size > 0)
    {
        Packet.Data = new char[size];
        std::memcpy(Packet.Data, data, size);
        Packet.Header.Length = HEADERSIZE + size + CRCSIZE;
    }
    else
    {
        Packet.Header.Length = HEADERSIZE + CRCSIZE;
    }
}

void PktDef::SetPktCount(int value)
{
    Packet.Header.PktCount = static_cast<uint8_t>(value);
}

PktDef::CmdType PktDef::GetCmd() const
{
    if (Packet.Header.Drive)
    {
        return DRIVE;
    }

    if (Packet.Header.Sleep)
    {
        return SLEEP;
    }

    return RESPONSE;
}

bool PktDef::GetAck() const
{
    return Packet.Header.Ack != 0;
}

int PktDef::GetLength() const
{
    return Packet.Header.Length;
}

char* PktDef::GetBodyData() const
{
    return Packet.Data;
}

int PktDef::GetPktCount() const
{
    return Packet.Header.PktCount;
}

unsigned char PktDef::countSetBits(unsigned char value) const
{
    unsigned char count = 0;

    while (value > 0)
    {
        count += (value & 1);
        value >>= 1;
    }

    return count;
}

unsigned char PktDef::calculateCRCFromBuffer(const char* buffer, int sizeWithoutCRC) const
{
    unsigned char crc = 0;

    for (int i = 0; i < sizeWithoutCRC; i++)
    {
        crc += countSetBits(static_cast<unsigned char>(buffer[i]));
    }

    return crc;
}

void PktDef::CalcCRC()
{
    int totalLength = Packet.Header.Length;

    if (totalLength < HEADERSIZE + CRCSIZE)
    {
        Packet.CRC = 0;
        return;
    }

    int sizeWithoutCRC = totalLength - CRCSIZE;
    int bodySize = totalLength - HEADERSIZE - CRCSIZE;

    char* tempBuffer = new char[sizeWithoutCRC];

    char* serializedHeader = Packet.Header.serialize();
    std::memcpy(tempBuffer, serializedHeader, HEADERSIZE);
    delete[] serializedHeader;

    if (bodySize > 0 && Packet.Data != nullptr)
    {
        std::memcpy(tempBuffer + HEADERSIZE, Packet.Data, bodySize);
    }

    Packet.CRC = static_cast<char>(calculateCRCFromBuffer(tempBuffer, sizeWithoutCRC));

    delete[] tempBuffer;
}

bool PktDef::CheckCRC(char* data, int size) const
{
    if (data == nullptr || size < HEADERSIZE + CRCSIZE)
    {
        return false;
    }

    unsigned char calculated = calculateCRCFromBuffer(data, size - CRCSIZE);
    unsigned char received = static_cast<unsigned char>(data[size - 1]);

    return calculated == received;
}

char* PktDef::GenPacket()
{
    delete[] RawBuffer;
    RawBuffer = nullptr;

    int totalLength = Packet.Header.Length;

    if (totalLength < HEADERSIZE + CRCSIZE)
    {
        totalLength = HEADERSIZE + CRCSIZE;
        Packet.Header.Length = totalLength;
    }

    CalcCRC();

    RawBuffer = new char[totalLength];

    char* serializedHeader = Packet.Header.serialize();
    std::memcpy(RawBuffer, serializedHeader, HEADERSIZE);
    delete[] serializedHeader;

    int bodySize = totalLength - HEADERSIZE - CRCSIZE;

    if (bodySize > 0 && Packet.Data != nullptr)
    {
        std::memcpy(RawBuffer + HEADERSIZE, Packet.Data, bodySize);
    }

    RawBuffer[totalLength - 1] = Packet.CRC;

    return RawBuffer;
}