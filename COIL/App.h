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

#include <mutex>

static App* app = nullptr;
static std::mutex app_mutex;

inline App* GetAppInstance() {
    std::lock_guard<std::mutex> lock(app_mutex);
    return app;
}

inline void startAppInstance() {
    std::lock_guard<std::mutex> lock(app_mutex);

    // If it exists, clean up the old one first
    if (app != nullptr) {
        delete app;
    }

    app = new App();
}

inline void restartAppInstance() {
    std::lock_guard<std::mutex> lock(app_mutex);
    if (app != nullptr) {
        app->restartCommand = true;
    }
}