#include "Robot.h"

Robot::Robot(int id)
{
	this->id = id;
	this->socket = nullptr;
}

Robot::~Robot()
{
	if (this->socket) {
		delete this->socket;
	}
}
