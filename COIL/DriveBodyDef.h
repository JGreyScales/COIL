#ifndef DRIVEBODY_DEF_H
#define DRIVEBODY_DEF_H

#include <iostream>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1)

class DriveBodyDef {
public:
    uint8_t Direction : 8; // 1-byte: FORWARD(1) or BACKWARD(2) 
    uint8_t Duration : 8;  // 1-byte in seconds 
    uint8_t Power : 8;     // 1-byte 80-100% 

    DriveBodyDef();
    ~DriveBodyDef();

    char* serialize();
    static DriveBodyDef* deserialize(char* data);

    void display();
};

#pragma pack(pop)

#endif