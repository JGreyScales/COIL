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

    bool restartCommand = false;
    bool hasDataToSend = false;
    char* dataBuffer = nullptr;

	App();

    void static attachRobotID(int, char*);
    int static detachRobotID(char*);
    

    PktDef* sendData(char*, bool);
    char * awaitReturn(MySocket*, char*);
    char * forwardToNextHop(char*);
    char * sendToRobot(Robot*, char*);

    bool openListeners();

    void lock(int);
    void unlock();

    ~App();
};

static App* app = nullptr;

inline App* GetAppInstance() {
    return app;
}

inline void startAppInstance() {
    if (app == nullptr) {
        app = new App();
    }
    else {
        delete app;
        app = new App();
    }
}

inline void restartAppInstance() {
    if (app != nullptr) {
        app->restartCommand = true;
    }
}