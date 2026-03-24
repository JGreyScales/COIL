#pragma once
class BodyInterface
{
public:
	virtual int getSize() = 0;
	virtual char* serialize() = 0;
	virtual ~BodyInterface() {};
	virtual void display() = 0;
};

