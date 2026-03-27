#include <iostream>
#include <bitset>

#include "Utils.h"
#include "HeaderDef.h"

HeaderDef::HeaderDef()
{
    this->PktCount = 1;
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
    char* buffer = new char[HEADER_SIZE];

    // Byte 0 & 1: Packet Count (Big Endian)
    buffer[1] = static_cast<char>((PktCount >> 8) & 0xFF);
    buffer[0] = static_cast<char>(PktCount & 0xFF);

    // Byte 2: Flags and Padding
    uint8_t flags = 0;
    flags |= (Drive & 0x01);           // Bit 0
    flags |= (Status & 0x01) << 1;     // Bit 1
    flags |= (Sleep & 0x01) << 2;     // Bit 2
    flags |= (Ack & 0x01) << 3;     // Bit 3
    flags |= (Padding & 0x0F) << 4;    // Bits 4-7
    buffer[2] = static_cast<char>(flags);

    // Byte 3: Length
    buffer[3] = static_cast<char>(Length);

    return buffer;
}

HeaderDef* HeaderDef::deserialize(char* data) {
    HeaderDef* res = new HeaderDef();
    if (!data) return res;

    // Byte 0 & 1: Reconstruct PktCount
    res->PktCount = (static_cast<uint8_t>(data[1]) << 8) | static_cast<uint8_t>(data[0]);

    // Byte 2: Extract Flags

    uint8_t flags = static_cast<uint8_t>(data[2]);

    res->Drive = (flags & 0x01);          // Extract bit 0 (previously bit 7)
    res->Status = (flags >> 1) & 0x01;     // Extract bit 1 (previously bit 6)
    res->Sleep = (flags >> 2) & 0x01;     // Extract bit 2 (previously bit 5)
    res->Ack = (flags >> 3) & 0x01;     // Extract bit 3 (previously bit 4)
    res->Padding = (flags >> 4) & 0x0F;     // Extract bits 4-7 (previously bits 0-3)

    // Byte 3: Length
    res->Length = static_cast<uint8_t>(data[3]);

    return res;
}



// ai generated dev helper function
void HeaderDef::display() {
    std::cout << "--- Header Data ---" << std::endl;

    std::cout << "Packet count: " << (int)PktCount
        << " [Binary: " << std::bitset<16>(PktCount) << "]" << std::endl;

    std::cout << "Drive Status: " << (int)Drive << " [Bit: " << (int)Drive << "]" << std::endl;
    std::cout << "Status:       " << (int)Status << " [Bit: " << (int)Status << "]" << std::endl;
    std::cout << "Sleep:        " << (int)Sleep << " [Bit: " << (int)Sleep << "]" << std::endl;
    std::cout << "Ack:          " << (int)Ack << " [Bit: " << (int)Ack << "]" << std::endl;

    std::cout << "Padding:      " << (int)Padding << " [Bits: " << std::bitset<4>(Padding) << "]" << std::endl;

    std::cout << "Length:       " << (int)Length
        << " [Binary: " << std::bitset<8>(Length) << "]" << std::endl;

    char* serializedData = this->serialize();

    std::cout << "\n--- Actual Serialized Layout (Bytes) ---" << std::endl;
    for (int i = 0; i < HEADER_SIZE; ++i) {
        std::cout << "Byte " << i << ": " << std::bitset<8>(static_cast<uint8_t>(serializedData[i])) << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}


HeaderDef::~HeaderDef()
{
}
