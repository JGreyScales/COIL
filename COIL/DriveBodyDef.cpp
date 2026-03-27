#include "DriveBodyDef.h"
#include <iostream>
#include <bitset>

DriveBodyDef::DriveBodyDef() : Direction(0), Duration(0), Power(0) {
    // Using an initializer list is slightly more idiomatic C++
}

DriveBodyDef::~DriveBodyDef() {}

int DriveBodyDef::getSize() {
    return DRIVE_BODY_SIZE;
}

char* DriveBodyDef::serialize() {
    char* buffer = new char[DRIVE_BODY_SIZE];

    // Explicitly casting to ensure correct byte assignment
    buffer[0] = static_cast<char>(this->Direction);
    buffer[1] = static_cast<char>(this->Duration);
    buffer[2] = static_cast<char>(this->Power);

    return buffer;
}

DriveBodyDef* DriveBodyDef::deserialize(char* data) {
    DriveBodyDef* returnValue = new DriveBodyDef();

    if (data) {
        returnValue->Direction = static_cast<uint8_t>(data[0]);
        returnValue->Duration = static_cast<uint8_t>(data[1]);
        returnValue->Power = static_cast<uint8_t>(data[2]);
    }

    return returnValue;
}

void DriveBodyDef::display() {
    std::cout << "--- DriveBody Data ---" << std::endl;
    std::cout << "Direction: " << (int)Direction << " [Binary: " << std::bitset<8>(Direction) << "]" << std::endl;
    std::cout << "Duration:  " << (int)Duration << " [Binary: " << std::bitset<8>(Duration) << "]" << std::endl;
    std::cout << "Power:     " << (int)Power << " [Binary: " << std::bitset<8>(Power) << "]" << std::endl;

    char* serializedData = this->serialize();

    std::cout << "\n--- Actual Serialized Layout (Bytes) ---" << std::endl;
    for (int i = 0; i < DRIVE_BODY_SIZE; ++i) {
        // Cast to uint8_t before bitset to avoid sign extension issues with char
        uint8_t byteVal = static_cast<uint8_t>(serializedData[i]);
        std::cout << "Byte " << i << ": " << std::bitset<8>(byteVal) << std::endl;
    }

    delete[] serializedData; // Clean up the buffer allocated in serialize()
    std::cout << "-------------------" << std::endl;
}