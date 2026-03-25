#pragma once


#include <cstdint>
#include "MySocket.h"

class Robot
{
public:
	uint8_t id;
	MySocket* socket;

	Robot(int);
};

