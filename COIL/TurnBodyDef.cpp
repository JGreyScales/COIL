#include <iostream>
#include <bitset>

#include "TurnBodyDef.h"

TurnBodyDef::TurnBodyDef()
{
    this->Direction = 0;
    this->Duration = 0;
}

int TurnBodyDef::getSize() {
    return TURN_BODY_SIZE;
}

char* TurnBodyDef::serialize() {
    char* buffer = new char[TurnBodyDef::TURN_BODY_SIZE];
    
    buffer[0] = Direction;
    // high bits
    buffer[1] = (Duration >> 8);
    // low bits
    buffer[2] = Duration;

    return buffer;
}

TurnBodyDef* TurnBodyDef::deserialize(char* data) {
    TurnBodyDef* returnValue = new TurnBodyDef();

    returnValue->Direction = data[0];
    // grab the high bits, and move them into the high bit position
    // cast them into unsigned to prevent corruption
    returnValue->Duration = (uint8_t)data[1] << 8 | (uint8_t)data[2];

    return returnValue;
}



// ai generated dev helper function
void TurnBodyDef::display() {
    std::cout << "--- TurnBody Data ---" << std::endl;

    std::cout << "Direction: " << (int)Direction << " [Binary: " << std::bitset<8>(Direction) << "]" << std::endl;
    std::cout << "Duration: " << (int)Duration << " [Binary: " << std::bitset<16>(Duration) << "]" << std::endl;

    uint8_t* rawData = reinterpret_cast<uint8_t*>(this);

    std::cout << "\n--- Raw Memory Layout (Bytes) ---" << std::endl;
    for (size_t i = 0; i < sizeof(TurnBodyDef); ++i) {
        std::cout << "Byte " << i << ": " << std::bitset<8>(rawData[i]) << std::endl;
    }   

    std::cout << "-------------------" << std::endl;
}


TurnBodyDef::~TurnBodyDef()
{
}
