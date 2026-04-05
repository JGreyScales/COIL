#include <iostream>
#include <bitset>
#include "TurnBodyDef.h"

TurnBodyDef::TurnBodyDef() : Direction(0), Duration(0) {
}

TurnBodyDef::~TurnBodyDef() {
}

int TurnBodyDef::getSize() {
    return TURN_BODY_SIZE;
}

char* TurnBodyDef::serialize() {
    char* buffer = new char[1024];

    buffer[0] = static_cast<char>(Direction);

    buffer[2] = static_cast<char>((Duration >> 8) & 0xFF);

    buffer[1] = static_cast<char>(Duration & 0xFF);

    return buffer;
}

TurnBodyDef* TurnBodyDef::deserialize(char* data) {
    TurnBodyDef* returnValue = new TurnBodyDef();

    if (data) {
        returnValue->Direction = static_cast<uint8_t>(data[0]);

        uint8_t highByte = static_cast<uint8_t>(data[2]);
        uint8_t lowByte = static_cast<uint8_t>(data[1]);

        returnValue->Duration = (static_cast<uint16_t>(highByte) << 8) | lowByte;
    }

    return returnValue;
}

void TurnBodyDef::display() {
    std::cout << "--- TurnBody Data ---" << std::endl;
    std::cout << "Direction: " << (int)Direction << " [Binary: " << std::bitset<8>(Direction) << "]" << std::endl;
    std::cout << "Duration:  " << (int)Duration << " [Binary: " << std::bitset<16>(Duration) << "]" << std::endl;

    char* serializedData = this->serialize();

    std::cout << "\n--- Actual Serialized Layout (Bytes) ---" << std::endl;
    for (int i = 0; i < TURN_BODY_SIZE; ++i) {
        uint8_t byteVal = static_cast<uint8_t>(serializedData[i]);
        std::cout << "Byte " << i << ": " << std::bitset<8>(byteVal) << std::endl;
    }

    delete[] serializedData;
    std::cout << "-------------------" << std::endl;
}