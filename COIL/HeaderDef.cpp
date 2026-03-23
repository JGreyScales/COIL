#include <iostream>
#include <bitset>

#include "Utils.h"


class HeaderDef
{
public:
    uint8_t PktCount : 8;


    uint8_t Drive : 1;
    uint8_t Status : 1;
    uint8_t Sleep : 1;
    uint8_t Ack : 1;
    uint8_t Padding : 4;

    uint8_t Length : 8;

    HeaderDef();

    char* serialize();
    static HeaderDef* deserialize(char* data);
    void display();

    ~HeaderDef();
};

HeaderDef::HeaderDef()
{
    this->PktCount = 0;
    this->Drive = false;
    this->Status = false;
    this->Sleep = false;
    this->Ack = false;
    this->Padding = 0;
    this->Length = sizeof(HeaderDef);
}

char* HeaderDef::serialize() {
    size_t size = sizeof(HeaderDef);
    char* buffer = new char[size];

    std::memcpy(buffer, this, sizeof(HeaderDef));
    buffer[1] = BitUtils::ReverseByte(buffer[1]);
    return buffer;
}

HeaderDef* HeaderDef::deserialize(char* data){
    HeaderDef* returnValue =  new HeaderDef();

    // because we know data order, and that this struct is packed we can just dump it into our class memory
    data[1] = BitUtils::ReverseByte(data[1]);
    std::memcpy(returnValue, data, sizeof(HeaderDef));

    return returnValue;
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
    for (size_t i = 0; i < sizeof(HeaderDef); ++i) {
        std::cout << "Byte " << i << ": " << std::bitset<8>(rawData[i]) << std::endl;
    }

    std::cout << "-------------------" << std::endl;
}


HeaderDef::~HeaderDef()
{
}
