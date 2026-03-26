#ifndef ACK_BODY_DEF_H
#define ACK_BODY_DEF_H

#include "BodyInterface.h"
#include <iostream>
#include <cstring>

#ifndef _WIN32

#define strcpy_s(dest, size, src) strncpy(dest, src, size)
#define strcat_s(dest, size, src) strncat(dest, src, size)

#endif

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
