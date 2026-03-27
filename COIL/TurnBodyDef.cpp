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
    char* buffer = new char[TURN_BODY_SIZE];

    // Byte 0: Direction
    buffer[0] = static_cast<char>(Direction);

    // Byte 1: High bits of Duration (Shift right by 8)
    buffer[1] = static_cast<char>((Duration >> 8) & 0xFF);

    // Byte 2: Low bits of Duration
    buffer[2] = static_cast<char>(Duration & 0xFF);

    return buffer;
}

TurnBodyDef* TurnBodyDef::deserialize(char* data) {
    TurnBodyDef* returnValue = new TurnBodyDef();

    if (data) {
        returnValue->Direction = static_cast<uint8_t>(data[0]);

        // Reconstruct Duration: 
        // Cast to uint8_t first to avoid sign extension, then shift and OR
        uint8_t highByte = static_cast<uint8_t>(data[1]);
        uint8_t lowByte = static_cast<uint8_t>(data[2]);

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

    delete[] serializedData; // Important: Clean up the buffer
    std::cout << "-------------------" << std::endl;
}