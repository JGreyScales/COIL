#pragma once
#include "Robot.h"
#include "PktDef.h"

enum appMode
{
    Primary,
    Secondary
};

class App
{
    Robot* robots[2] = { new Robot(0), new Robot(1) };
    MySocket* curSoc;
    appMode mode;

	App();

    void attachRobotID(int, char*);
    int detachRobotID(char*);
    
    char * awaitReturn();
    void forwardToNextHop();
    void sendToRobot();

    bool openListeners();

    ~App();
};

