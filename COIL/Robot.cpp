#include "Robot.h"

Robot::Robot(int id)
{
	this->id = id;
}

Robot::~Robot()
{
	if (this->socket) {
		delete this->socket;
	}
}
