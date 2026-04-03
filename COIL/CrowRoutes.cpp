#include "CrowRoutes.h"

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <cstring>
#include <stdexcept>

void CrowRoutes::RegisterRoutes(crow::SimpleApp& app)
{
    CROW_ROUTE(app, "/")
        ([]() {
        return CrowRoutes::HandleRoot();
            });

    CROW_ROUTE(app, "/commands/Drive/<int>")
        .methods(crow::HTTPMethod::Get)
        ([](const crow::request& req, int direction) {
        return CrowRoutes::HandleDrive(req, direction);
            });

    CROW_ROUTE(app, "/commands/telem")
        .methods(crow::HTTPMethod::Get)
        ([](const crow::request& req) {
        return CrowRoutes::HandleTelemetry(req);
            });

    // sleep
    CROW_ROUTE(app, "/commands/sleep")
        .methods(crow::HTTPMethod::Get)
        ([](const crow::request& req) {
		return CrowRoutes::HandleSleep(req); 
			});

    // and restart
    CROW_ROUTE(app, "/commands/restart")
        .methods(crow::HTTPMethod::Get)
		([](const crow::request& req) {
		return CrowRoutes::HandleRestart(req);
			});
    // needed
}

crow::response CrowRoutes::HandleRoot()
{
    fileFetcher ff;
    std::string html = ff.sendFile("index.html");

    if (html == "not found")
    {
        return crow::response(404, "Not Found");
    }

    crow::response res(200, html);
    res.set_header("Content-Type", "text/html");
    return res;
}

crow::response CrowRoutes::HandleDrive(const crow::request& req, int direction)
{
    std::string robotIdStr = req.url_params.get("robotId") ? req.url_params.get("robotId") : "0";
    std::string durationStr = req.url_params.get("duration") ? req.url_params.get("duration") : "0";
    std::string powerStr = req.url_params.get("power") ? req.url_params.get("power") : "0";

    int robotId = std::stoi(robotIdStr);
    int duration = std::stoi(durationStr);
    int power = std::stoi(powerStr);

    PktDef* pktInstance = new PktDef();
    pktInstance->SetCmd(PktDef::CmdType::DRIVE);

    BodyInterface* body = nullptr;

    switch (direction) {
    case 1:
    case 2:
    {
        DriveBodyDef* driveBody = new DriveBodyDef();
        driveBody->Direction = direction;
        driveBody->Duration = duration;
        driveBody->Power = power;
        body = driveBody;
        break;
    }
    case 3:
    case 4:
    {
        TurnBodyDef* turnBody = new TurnBodyDef();
        turnBody->Direction = direction;
        turnBody->Duration = duration;
        body = turnBody;
        break;
    }
    default:
        body = nullptr;
        break;
    }

    if (body != nullptr) {
        pktInstance->SetBodyData(body);
    }
    else {
        delete pktInstance;
        return crow::response(400, "Invalid Direction");
    }

    char* serializedDataToSend = pktInstance->GenPacket();
    int pktLen = pktInstance->GetLength();
    int bufferLength = MySocket::DEFAULT_SIZE;

    char* sendBuffer = new char[bufferLength];
    memset(sendBuffer, 0, bufferLength);
    memcpy(sendBuffer, serializedDataToSend, pktLen);

    App::attachRobotID(robotId, sendBuffer);

    App* app = GetAppInstance();

    PktDef* pktResponse = app->sendData(sendBuffer);


    pktResponse->display();

    // 6. CLEANUP: Avoid memory leaks
    delete pktInstance; // This also cleans up the body
    // we do not delete send buffer, because sendData already cleaned that up

    return crow::response(200, "Command Sent");
}

crow::response CrowRoutes::HandleTelemetry(const crow::request& req)
{
    return crow::response(200, "OK");
}

crow::response CrowRoutes::HandleSleep(const crow::request& req)
{
  
    return crow::response(200, "OK");

}

crow::response CrowRoutes::HandleRestart(const crow::request& req)
{
    return crow::response(200, "OK");

}
