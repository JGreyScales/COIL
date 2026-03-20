#include "DriveBodyDef.h"
#include <iostream>
#include <bitset>

DriveBodyDef::DriveBodyDef() {
    this->Direction = 0;
    this->Duration = 0;
    this->Power = 0;
}

DriveBodyDef::~DriveBodyDef() {}

char* DriveBodyDef::serialize() {
    // Allocates a buffer of 3 bytes and copies the struct content
    char* buffer = new char[sizeof(DriveBodyDef)];
    std::memcpy(buffer, this, sizeof(DriveBodyDef));
    return buffer;
}

DriveBodyDef* DriveBodyDef::deserialize(char* data) {
    DriveBodyDef* returnValue = new DriveBodyDef();
    // Directly maps raw data into the object memory
    std::memcpy(returnValue, data, sizeof(DriveBodyDef));
    return returnValue;
}
 
void DriveBodyDef::display() {
    std::cout << "--- DriveBody Data ---" << std::endl;
    std::cout << "Direction: " << (int)Direction << " [Binary: " << std::bitset<8>(Direction) << "]" << std::endl;
    std::cout << "Duration:  " << (int)Duration  << " [Binary: " << std::bitset<8>(Duration) << "]" << std::endl;
    std::cout << "Power:     " << (int)Power     << " [Binary: " << std::bitset<8>(Power) << "]" << std::endl;

    uint8_t* rawData = reinterpret_cast<uint8_t*>(this);
    std::cout << "\n--- Raw Memory Layout (Bytes) ---" << std::endl;
    for (size_t i = 0; i < sizeof(DriveBodyDef); ++i) {
        std::cout << "Byte " << i << ": " << std::bitset<8>(rawData[i]) << std::endl;
    }
    std::cout << "----------------------" << std::endl;
}