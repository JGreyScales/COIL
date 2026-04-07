#include "CrowRoutes.h"

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <cstring>
#include <stdexcept>

int CrowRoutes::robotCounts[2] = { 0, 0 };

crow::response CrowRoutes::JsonResponse(int code, const crow::json::wvalue& body)
{
    crow::response res;
    res.code = code;
    res.set_header("Content-Type", "application/json");
    res.write(body.dump());
    return res;
}

crow::json::wvalue CrowRoutes::BuildCommandJson(PktDef* pktResponse)
{
    crow::json::wvalue result;
    result["ack"] = pktResponse->GetAck() ? 1 : 0;

    char* responseBody = pktResponse->GetBodyData();
    int bodySize = pktResponse->GetLength() - PktDef::CRCSIZE - PktDef::HEADERSIZE;
    if (responseBody != nullptr)
    {
        result["body"] = std::string(responseBody, bodySize);
        delete[] responseBody;
    }
    else
    {
        result["body"] = "";
    }

    return result;
}

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
    {
        DriveBodyDef* driveBody = new DriveBodyDef();
        driveBody->Direction = direction;
        driveBody->Duration = duration;
        driveBody->Power = power;
        body = driveBody;
        break;
    }
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
    {
        TurnBodyDef* turnBody = new TurnBodyDef();
        turnBody->Direction = direction;
        turnBody->Duration = duration;
        body = turnBody;
        break;
    }
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
        crow::json::wvalue err;
        err["ack"] = 0;
        err["body"] = "Invalid Direction";
        return JsonResponse(400, err);
    }

    char* serializedDataToSend = pktInstance->GenPacket();
    int pktLen = pktInstance->GetLength();
    int bufferLength = MySocket::DEFAULT_SIZE;

    char* sendBuffer = new char[bufferLength];
    memset(sendBuffer, 0, bufferLength);
    memcpy(sendBuffer, serializedDataToSend, pktLen);

    App::attachRobotID(robotId, sendBuffer);

    App* app = GetAppInstance();

    robotCounts[robotId]++;

    PktDef* pktResponse = app->sendData(sendBuffer, false);

    if (pktResponse == nullptr)
    {
        delete pktInstance;

        crow::json::wvalue err;
        err["ack"] = 0;
        err["body"] = "No response returned from drive command";
        return JsonResponse(500, err);
    }

    pktResponse->display();

    crow::json::wvalue result = BuildCommandJson(pktResponse);

    // 6. CLEANUP: Avoid memory leaks
    delete pktInstance; // This also cleans up the body
    // we do not delete send buffer, because sendData already cleaned that up

    return JsonResponse(200, result);
}

crow::response CrowRoutes::HandleTelemetry(const crow::request& req)
{
    try
    {
        std::string robotIdStr = req.url_params.get("robotId") ? req.url_params.get("robotId") : "0";
        int robotId = std::stoi(robotIdStr);

        if (!ConsoleUtils::validateRobotId(robotId))
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "robotId must be 0 or 1";
            return JsonResponse(400, err);
        }     

        HeaderDef header;
        header.PktCount = robotCounts[robotId];
        header.Drive = 0;
        header.Sleep = 0;
        header.Ack = 0;
        header.Status = 1;
        header.Padding = 0;
        header.Length = HeaderDef::HEADER_SIZE + PktDef::CRCSIZE;

        char* headerBytes = header.serialize();
;
        char* sendBuffer = new char[MySocket::DEFAULT_SIZE];
        std::memset(sendBuffer, 0, MySocket::DEFAULT_SIZE);
        std::memcpy(sendBuffer, headerBytes, HeaderDef::HEADER_SIZE);

        delete[] headerBytes;

        PktDef helper;
        sendBuffer[header.Length - 1] = static_cast<char>(helper.countSetBits(sendBuffer, PktDef::HEADERSIZE + PktDef::CRCSIZE));

        App::attachRobotID(robotId, sendBuffer);

        App* app = GetAppInstance();
        if (app == nullptr)
        {
            delete[] sendBuffer;

            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "App instance is not running";
            return JsonResponse(500, err);
        }

        robotCounts[robotId]++;
        PktDef* pktResponse = app->sendData(sendBuffer, false);


        if (pktResponse == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "No telemetry response returned";
            return JsonResponse(500, err);
        }

        pktResponse->display();

        crow::json::wvalue result;
        result["ack"] = pktResponse->GetAck() ? 1 : 0;

        char* bodyData = pktResponse->GetBodyData();
        if (bodyData != nullptr)
        {
            TelemetryBodyDef* telem = TelemetryBodyDef::deserialize(bodyData);
            delete[] bodyData;

            result["LastPktCounter"] = telem->LastPktCounter;
            result["CurrentGrade"] = telem->CurrentGrade;
            result["HitCount"] = telem->HitCount;
            result["Heading"] = telem->Heading;
            result["LastCmd"] = telem->LastCmd;
            result["LastCmdValue"] = telem->LastCmdValue;
            result["LastCmdPower"] = telem->LastCmdPower;

            delete telem;
        }
        else
        {
            result["LastPktCounter"] = 0;
            result["CurrentGrade"] = 0;
            result["HitCount"] = 0;
            result["Heading"] = 0;
            result["LastCmd"] = 0;
            result["LastCmdValue"] = 0;
            result["LastCmdPower"] = 0;
        }

        return JsonResponse(200, result);
    }
    catch (const std::exception& ex)
    {
        crow::json::wvalue err;
        err["ack"] = 0;
        err["body"] = std::string(ex.what());
        return JsonResponse(500, err);
    }
}

crow::response CrowRoutes::HandleSleep(const crow::request& req)
{
    try
    {
        std::string robotIdStr = req.url_params.get("robotId") ? req.url_params.get("robotId") : "0";
        int robotId = std::stoi(robotIdStr);

        if (!ConsoleUtils::validateRobotId(robotId))
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "robotId must be 0 or 1";
            return JsonResponse(400, err);
        }

        PktDef* pktInstance = new PktDef();
        pktInstance->SetCmd(PktDef::CmdType::SLEEP);
        pktInstance->SetBodyData(nullptr);

        char* serializedDataToSend = pktInstance->GenPacket();
        int pktLen = pktInstance->GetLength();

        char* sendBuffer = new char[MySocket::DEFAULT_SIZE];
        std::memset(sendBuffer, 0, pktLen + 1);
        std::memcpy(sendBuffer, serializedDataToSend, pktLen);

        App::attachRobotID(robotId, sendBuffer);

        App* app = GetAppInstance();
        if (app == nullptr)
        {
            delete pktInstance;

            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "App instance is not running";
            return JsonResponse(500, err);
        }

        app->sendData(sendBuffer, false);
        

        app->restartCommand = true;
        return JsonResponse(200, "Good Night");
    }
    catch (const std::exception& ex)
    {
        crow::json::wvalue err;
        err["ack"] = 0;
        err["body"] = std::string(ex.what());
        return JsonResponse(500, err);
    }
}

crow::response CrowRoutes::HandleRestart(const crow::request& req)
{
    App* app = GetAppInstance();
    app->restartCommand = true;
    return JsonResponse(200, "yay");
}