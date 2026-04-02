#pragma once
#include "Robot.h"
#include "PktDef.h"

#include <thread>
#include <chrono>

enum appMode
{
    Primary,
    Secondary
};

class App
{
    Robot* robots[2] = { nullptr, nullptr };
    MySocket* curSoc; // inbound
    MySocket* nexthop; // outbound
    appMode mode;
    MySocket::ConnectionType connectionType;
    static inline bool networkLock = false;

public:

    bool hasDataToSend = false;
    char* dataBuffer;

    App();
    App(bool webMode);

    void static attachRobotID(int, char*);
    int static detachRobotID(char*);

    PktDef* sendData(char*, bool);
    char* awaitReturn(MySocket*, char*);
    char* forwardToNextHop(char*);
    char* sendToRobot(Robot*, char*);

    bool openListeners();

    void lock(int);
    void unlock();

    ~App();
};