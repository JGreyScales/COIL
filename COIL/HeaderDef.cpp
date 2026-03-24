#include <iostream>
#include <bitset>

#include "Utils.h"
#include "HeaderDef.h"

HeaderDef::HeaderDef()
{
    this->PktCount = 0;
    this->Drive = false;
    this->Status = false;
    this->Sleep = false;
    this->Ack = false;
    this->Padding = 0;
    this->Length = 0;
}

int HeaderDef::getSize() {
    return HEADER_SIZE;
}

char* HeaderDef::serialize() {

    char* buffer = new char[HeaderDef::HEADER_SIZE];

    buffer[0] = (this->PktCount >> 8);
    buffer[1] = (this->PktCount);

    uint8_t flags = 0;
    // low bits
    flags |= (this->Padding & 0x0F) << 0;
    flags |= (this->Ack & 0x01) << 4; 
    flags |= (this->Sleep & 0x01) << 5;
    flags |= (this->Status & 0x01) << 6;
    flags |= (this->Drive & 0x01) << 7;
    // high bits
    buffer[2] = flags;

    buffer[3] = this->Length;

    return buffer;
}

HeaderDef* HeaderDef::deserialize(char* data) {
    HeaderDef* res = new HeaderDef();
    if (!data) return res;

    res->PktCount = ((uint8_t)data[0] << 8 | (uint8_t)data[1]);

    uint8_t flags = data[2];
    res->Padding = (flags >> 0) & 0x0F;
    res->Ack = (flags >> 4) & 0x01;
    res->Sleep = (flags >> 5) & 0x01;
    res->Status = (flags >> 6) & 0x01;
    res->Drive = (flags >> 7) & 0x01;

    res->Length = data[3];

    return res;
}



// ai generated dev helper function
void HeaderDef::display() {
    std::cout << "--- Header Data ---" << std::endl;

    std::cout << "Packet count: " << (int)PktCount
        << " [Binary: " << std::bitset<8>(PktCount) << "]" << std::endl;

    std::cout << "Drive Status: " << (int)Drive << " [Bit: " << (int)Drive << "]" << std::endl;
    std::cout << "Status:       " << (int)Status << " [Bit: " << (int)Status << "]" << std::endl;
    std::cout << "Sleep:        " << (int)Sleep << " [Bit: " << (int)Sleep << "]" << std::endl;
    std::cout << "Ack:          " << (int)Ack << " [Bit: " << (int)Ack << "]" << std::endl;

    std::cout << "Padding:      " << (int)Padding << " [Bits: " << std::bitset<4>(Padding) << "]" << std::endl;

    std::cout << "Length:       " << (int)Length
        << " [Binary: " << std::bitset<8>(Length) << "]" << std::endl;

    // FIX: Access the raw memory of the struct to see the packing
    // We cast 'this' to a unsigned char pointer to look at the raw bytes.
    uint8_t* rawData = reinterpret_cast<uint8_t*>(this);

    std::cout << "\n--- Raw Memory Layout (Bytes) ---" << std::endl;
    for (size_t i = 0; i < HEADER_SIZE; ++i) {
        std::cout << "Byte " << i << ": " << std::bitset<8>(rawData[i]) << std::endl;
    }

    std::cout << "-------------------" << std::endl;
}


HeaderDef::~HeaderDef()
{
}
