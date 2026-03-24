#ifndef ACK_BODY_DEF_H
#define ACK_BODY_DEF_H

#include "BodyInterface.h"
#include <iostream>
#include <cstring>

class AckBodyDef : public BodyInterface {
public:
    char* body;

    AckBodyDef();
    ~AckBodyDef();

    int getSize() override;

    char* serialize() override;

    static AckBodyDef* deserialize(char* data);

    void display();

    void setBody(const char* newBody);
};

#endif 
