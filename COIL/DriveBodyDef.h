#ifndef DRIVEBODY_DEF_H
#define DRIVEBODY_DEF_H

#include <iostream>
#include <cstdint>
#include <cstring>
#include "BodyInterface.h"

class DriveBodyDef : public BodyInterface {
public:
    static const int DRIVE_BODY_SIZE = 3;

    int Direction; // 1-byte: FORWARD(1) or BACKWARD(2) 
    int Duration;  // 1-byte in seconds 
    int Power;     // 1-byte 80-100% 

    DriveBodyDef();
    virtual ~DriveBodyDef();

    int getSize() override;
    char* serialize() override;
    static DriveBodyDef* deserialize(char* data);

    void display();
};

#endif