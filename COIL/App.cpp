#include "App.h"


App::App() {

}


// for both of these functions, the raw buffer
void App::attachRobotID(int ID, char* data)
{
	PktDef* pktInstance = new PktDef(data);
	size_t length = pktInstance->GetLength();

	data[length + 1] = ID;

	delete pktInstance;
}

int App::detachRobotID(char* data)
{
	PktDef* pktInstance = new PktDef(data);
	size_t length = pktInstance->GetLength();

	int ID = data[length + 1];

	delete pktInstance;

	return ID;
}

char* App::awaitReturn()
{
	return nullptr;
}

void App::forwardToNextHop()
{
}

void App::sendToRobot()
{
}

bool App::openListeners()
{
	return false;
}

App::~App()
{
	delete this->robots[0];
	delete this->robots[0];

	delete this->curSoc;
}
