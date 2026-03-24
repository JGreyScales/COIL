#ifndef DRIVEBODY_DEF_H
#define DRIVEBODY_DEF_H

#include <iostream>
#include <cstdint>
#include <cstring>

#include "BodyInterface.h"

class DriveBodyDef : public BodyInterface {
public:
    static const int DRIVE_BODY_SIZE = 3;

    uint8_t Direction : 8; // 1-byte: FORWARD(1) or BACKWARD(2) 
    uint8_t Duration : 8;  // 1-byte in seconds 
    uint8_t Power : 8;     // 1-byte 80-100% 

    DriveBodyDef();
    ~DriveBodyDef();

    int getSize() override;
    char* serialize() override;
    static DriveBodyDef* deserialize(char* data);

    void display();
};


#endif