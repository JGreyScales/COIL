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
    this->Packet.Header = nullptr;
    this->Packet.body = nullptr;
    this->Packet.CRC = 0;
    this->cmd = CmdType::UNDEFINED;

    if (rawData == nullptr) return;

    // isolate the header data
    std::cout << "Isolated Header Bytes: " ;
    BitUtils::displayChar(4, rawData);
    std::cout << std::endl;

    this->Packet.Header = HeaderDef::deserialize(rawData);

    if (this->Packet.Header == nullptr) return;

    if (this->Packet.Header->Status || this->Packet.Header->Ack) {
        this->cmd = CmdType::RESPONSE;
    } else if (this->Packet.Header->Drive) {
        this->cmd = CmdType::DRIVE;
    }
    else if (this->Packet.Header->Sleep) {
        this->cmd = CmdType::SLEEP;
    }
    else {
        this->cmd = CmdType::UNDEFINED;
    }


    int bodySize = this->Packet.Header->Length - HEADERSIZE - CRCSIZE;

    std::cout << "Isolated Body Bytes: ";
    BitUtils::displayChar(bodySize, rawData + HEADERSIZE);
    std::cout << std::endl;

    if (bodySize > 0)
    {
        char* bodyBuffer = new char[bodySize];
        std::memcpy(bodyBuffer, rawData + HEADERSIZE, bodySize);

        // we will only recieve acks or tele responses
        if (this->cmd == CmdType::RESPONSE) {
            if (this->Packet.Header->Status) {
                this->Packet.body = TelemetryBodyDef::deserialize(bodyBuffer);
            }
            else {
                this->Packet.body = AckBodyDef::deserialize(bodyBuffer);
            }
        }
        else {
            this->Packet.body = nullptr;
        }


        delete[] bodyBuffer;
    }

    std::cout << "Isolated CRC Byte: ";
    BitUtils::displayChar(1, &rawData[this->Packet.Header->Length - 1]);
    std::cout << std::endl;
    this->Packet.CRC = static_cast<unsigned char>(rawData[this->Packet.Header->Length - 1]);
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
    Packet.Header->Status = 0; // just always request telementry data from endpoint

    switch (cmd)
    {
    case DRIVE:
        Packet.Header->Drive = 1;
        break;

    case SLEEP:
        Packet.Header->Sleep = 1;
        break;
    case RESPONSE:
        Packet.Header->Status = 1;
        Packet.Header->Ack = 1;
    default:
        break;
    }
}

void PktDef::SetBodyData(BodyInterface* data)
{
    this->Packet.body = data;
    if (!data) {
        // 0 is just to represent the 0 width body
        this->Packet.Header->Length = HEADERSIZE + 0 + CRCSIZE;
        return;
    }
    this->Packet.Header->Length = HEADERSIZE + data->getSize() + CRCSIZE;
}


void PktDef::SetPktCount(int value)
{
    if (0 > value) {
        return;
    }

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
    if (Packet.body) {
        return Packet.body->serialize();
    }
    else {
        return nullptr;
    }
    
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
        unsigned char byte = value[i];

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
    memset(RawBuffer, 0, totalLength);

    if (totalLength < HEADERSIZE + CRCSIZE) {
        Packet.CRC = 0;
        return;
    }

    int sizeWithoutCRC = totalLength - CRCSIZE;


    char* serializedHeader = Packet.Header->serialize();
    std::memcpy(this->RawBuffer, serializedHeader, HEADERSIZE);
    delete[] serializedHeader;

    if (Packet.body != nullptr) {
        int bodySize = Packet.body->getSize();

        if (bodySize > 0)
        {
            char* serializedBody = Packet.body->serialize();
            std::memcpy(this->RawBuffer + HEADERSIZE, serializedBody, bodySize);
            delete[] serializedBody;
        }
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
    unsigned char received = data[size - CRCSIZE];

    return calculated == received;
}
//
char* PktDef::GenPacket()
{
    unsigned int bodySize = 0;
    if (Packet.body != nullptr) {
        bodySize = Packet.body->getSize();
    }

    // size ts so that the the compiler knows the range of the ints
    const size_t totalLength = (size_t)HEADERSIZE + bodySize + (size_t)CRCSIZE;

    // somebullshit to make the compiler stop throwing warnings
    // it was complaining about a possible integer wrap
    // if the int value is too large (it wont be though)
    if (totalLength < (size_t)HEADERSIZE) {
        return nullptr;
    }

    Packet.Header->Length = (int)totalLength;


    CalcCRC();

    delete[] this->RawBuffer;
    RawBuffer = new char[totalLength];
    memset(RawBuffer, 0, totalLength);

    char* serializedHeader = Packet.Header->serialize();

    if (serializedHeader == nullptr) {
        return nullptr;
    }

    std::memcpy(this->RawBuffer, serializedHeader, (size_t)HEADERSIZE);
    delete[] serializedHeader;


    if (bodySize > 0)
    {
        char* serializedBody = Packet.body->serialize();
        std::memcpy(this->RawBuffer + HEADERSIZE, serializedBody, bodySize);
        delete[] serializedBody;
    }
   

    RawBuffer[totalLength - 1] = Packet.CRC;
    this->SetPktCount(this->GetPktCount() + 1);


    return RawBuffer;
}

void PktDef::display() {
    this->Packet.Header->display();
    if (this->Packet.body) {
        this->Packet.body->display();
    }
    std::cout << "CRC: " << (int)(this->Packet.CRC) << " [Binary: " << std::bitset<8>(this->Packet.CRC) << "]" << std::endl;
}