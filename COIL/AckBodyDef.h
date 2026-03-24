#ifndef ACK_BODY_DEF_H
#define ACK_BODY_DEF_H

#include <iostream>
#include <cstring>

class AckBodyDef {
public:
    char* body;

    AckBodyDef();
    ~AckBodyDef();

    char* serialize();

    static AckBodyDef* deserialize(char* data);

    void display();

    void setBody(const char* newBody);
};

#endif 
