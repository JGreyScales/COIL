#include "CrowRoutes.h"

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <cstring>
#include <stdexcept>

namespace
{
    std::mutex g_requestMutex;
    std::mutex g_responseMutex;
    std::condition_variable g_responseCv;

    PktDef* g_responsePacket = nullptr;
    bool g_responseReady = false;

    crow::response JsonResponse(int code, const crow::json::wvalue& body)
    {
        crow::response res;
        res.code = code;
        res.set_header("Content-Type", "application/json");
        res.write(body.dump());
        return res;
    }

    PktDef* CloneResponsePacket(PktDef* src)
    {
        if (src == nullptr)
        {
            return nullptr;
        }

        char* rawCopy = src->GetRawPacketCopy();
        if (rawCopy == nullptr)
        {
            return nullptr;
        }

        PktDef* cloned = new PktDef(rawCopy);
        cloned->deleteBuffer = true;
        return cloned;
    }

    PktDef* WaitForResponseOrThrow(int timeoutSeconds = 10)
    {
        std::unique_lock<std::mutex> lock(g_responseMutex);

        bool ok = g_responseCv.wait_for(
            lock,
            std::chrono::seconds(timeoutSeconds),
            []() { return g_responseReady; });

        if (!ok || g_responsePacket == nullptr)
        {
            throw std::runtime_error("Timed out waiting for robot response");
        }

        PktDef* result = g_responsePacket;
        g_responsePacket = nullptr;
        g_responseReady = false;
        return result;
    }

    char* BuildTelemetryRequestRaw()
    {
        HeaderDef header;
        header.PktCount = 1;
        header.Drive = 0;
        header.Sleep = 0;
        header.Ack = 0;
        header.Status = 1;
        header.Padding = 0;
        header.Length = HeaderDef::HEADER_SIZE + PktDef::CRCSIZE;

        char* headerBytes = header.serialize();

        // +1 because attachRobotID appends one byte after the packet
        char* raw = new char[header.Length + 1];
        std::memset(raw, 0, header.Length + 1);
        std::memcpy(raw, headerBytes, HeaderDef::HEADER_SIZE);

        PktDef helper;
        raw[header.Length - 1] =
            static_cast<char>(helper.countSetBits(raw, header.Length - PktDef::CRCSIZE));

        delete[] headerBytes;
        return raw;
    }

    crow::json::wvalue BuildMovementJson(PktDef* responsePacket)
    {
        crow::json::wvalue body;
        body["ack"] = responsePacket->GetAck() ? 1 : 0;

        char* responseBody = responsePacket->GetBodyData();
        if (responseBody != nullptr)
        {
            body["body"] = std::string(responseBody);
            delete[] responseBody;
        }
        else
        {
            body["body"] = "";
        }

        return body;
    }

    crow::json::wvalue BuildTelemetryJson(PktDef* responsePacket)
    {
        crow::json::wvalue body;
        body["ack"] = responsePacket->GetAck() ? 1 : 0;

        char* serializedBody = responsePacket->GetBodyData();
        if (serializedBody == nullptr)
        {
            body["LastPktCounter"] = 0;
            body["CurrentGrade"] = 0;
            body["HitCount"] = 0;
            body["Heading"] = 0;
            body["LastCmd"] = 0;
            body["LastCmdValue"] = 0;
            body["LastCmdPower"] = 0;
            return body;
        }

        TelemetryBodyDef* telem = TelemetryBodyDef::deserialize(serializedBody);
        delete[] serializedBody;

        body["LastPktCounter"] = static_cast<int>(telem->LastPktCounter);
        body["CurrentGrade"] = static_cast<int>(telem->CurrentGrade);
        body["HitCount"] = static_cast<int>(telem->HitCount);
        body["Heading"] = static_cast<int>(telem->Heading);
        body["LastCmd"] = static_cast<int>(telem->LastCmd);
        body["LastCmdValue"] = static_cast<int>(telem->LastCmdValue);
        body["LastCmdPower"] = static_cast<int>(telem->LastCmdPower);

        delete telem;
        return body;
    }
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
    try
    {
        std::cout << "HandleDrive route hit" << std::endl;
        const char* robotIdRaw = req.url_params.get("robotId");
        const char* durationRaw = req.url_params.get("duration");
        const char* powerRaw = req.url_params.get("power");

        if (robotIdRaw == nullptr || durationRaw == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "Missing robotId or duration";
            return JsonResponse(400, err);
        }

        int robotId = std::stoi(robotIdRaw);
        int duration = std::stoi(durationRaw);

        if (!ConsoleUtils::validateRobotId(robotId))
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "robotId must be 0 or 1";
            return JsonResponse(400, err);
        }

        if (!ConsoleUtils::validateDirection(direction))
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "direction must be 1 to 4";
            return JsonResponse(400, err);
        }

        if (!ConsoleUtils::validateDuration(duration))
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "duration must be between 1 and 100";
            return JsonResponse(400, err);
        }

        std::unique_lock<std::mutex> requestLock(g_requestMutex);

        App* app = GetAppInstance();
        if (app == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "App instance is not running";
            return JsonResponse(500, err);
        }

        {
            std::lock_guard<std::mutex> responseLock(g_responseMutex);
            if (g_responsePacket != nullptr)
            {
                delete g_responsePacket;
                g_responsePacket = nullptr;
            }
            g_responseReady = false;
        }

        PktDef outbound;
        outbound.SetCmd(PktDef::DRIVE);

        if (direction == PktDef::FORWARD || direction == PktDef::BACKWARD)
        {
            if (powerRaw == nullptr)
            {
                crow::json::wvalue err;
                err["ack"] = 0;
                err["body"] = "power is required for forward/backward";
                return JsonResponse(400, err);
            }

            int power = std::stoi(powerRaw);

            if (!ConsoleUtils::validatePower(power))
            {
                crow::json::wvalue err;
                err["ack"] = 0;
                err["body"] = "power must be between 80 and 100";
                return JsonResponse(400, err);
            }

            DriveBodyDef* body = new DriveBodyDef();
            body->Direction = direction;
            body->Duration = duration;
            body->Power = power;
            outbound.SetBodyData(body);
        }
        else
        {
            TurnBodyDef* body = new TurnBodyDef();
            body->Direction = static_cast<uint8_t>(direction);
            body->Duration = static_cast<uint16_t>(duration);
            outbound.SetBodyData(body);
        }

        char* rawBuffer = outbound.GenPacket();
        if (rawBuffer == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "Failed to build packet";
            return JsonResponse(500, err);
        }

        App::attachRobotID(robotId, rawBuffer);

        BitUtils::displayChar(outbound.GetLength() + 1, rawBuffer);

        app->dataBuffer = rawBuffer;
        app->hasDataToSend = true;

        std::cout << "Waiting for app response..." << std::endl;
        PktDef* responsePacket = WaitForResponseOrThrow();
        crow::json::wvalue result = BuildMovementJson(responsePacket);
        delete responsePacket;

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

crow::response CrowRoutes::HandleTelemetry(const crow::request& req)
{
    try
    {
        const char* robotIdRaw = req.url_params.get("robotId");
        if (robotIdRaw == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "Missing robotId";
            return JsonResponse(400, err);
        }

        int robotId = std::stoi(robotIdRaw);

        if (!ConsoleUtils::validateRobotId(robotId))
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "robotId must be 0 or 1";
            return JsonResponse(400, err);
        }

        std::unique_lock<std::mutex> requestLock(g_requestMutex);

        App* app = GetAppInstance();
        if (app == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "App instance is not running";
            return JsonResponse(500, err);
        }

        {
            std::lock_guard<std::mutex> responseLock(g_responseMutex);
            if (g_responsePacket != nullptr)
            {
                delete g_responsePacket;
                g_responsePacket = nullptr;
            }
            g_responseReady = false;
        }

        char* rawBuffer = BuildTelemetryRequestRaw();
        App::attachRobotID(robotId, rawBuffer);

        BitUtils::displayChar(HeaderDef::HEADER_SIZE + PktDef::CRCSIZE + 1, rawBuffer);

        app->dataBuffer = rawBuffer;
        app->hasDataToSend = true;

        PktDef* responsePacket = WaitForResponseOrThrow();
        crow::json::wvalue result = BuildTelemetryJson(responsePacket);
        delete responsePacket;

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
        const char* robotIdRaw = req.url_params.get("robotId");
        if (robotIdRaw == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "Missing robotId";
            return JsonResponse(400, err);
        }

        int robotId = std::stoi(robotIdRaw);

        if (!ConsoleUtils::validateRobotId(robotId))
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "robotId must be 0 or 1";
            return JsonResponse(400, err);
        }

        std::unique_lock<std::mutex> requestLock(g_requestMutex);

        App* app = GetAppInstance();
        if (app == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "App instance is not running";
            return JsonResponse(500, err);
        }

        {
            std::lock_guard<std::mutex> responseLock(g_responseMutex);
            if (g_responsePacket != nullptr)
            {
                delete g_responsePacket;
                g_responsePacket = nullptr;
            }
            g_responseReady = false;
        }

        PktDef outbound;
        outbound.SetCmd(PktDef::SLEEP);
        outbound.SetBodyData(nullptr);

        char* rawBuffer = outbound.GenPacket();
        if (rawBuffer == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "Failed to build sleep packet";
            return JsonResponse(500, err);
        }

        App::attachRobotID(robotId, rawBuffer);

        BitUtils::displayChar(outbound.GetLength() + 1, rawBuffer);

        app->dataBuffer = rawBuffer;
        app->hasDataToSend = true;

        PktDef* responsePacket = WaitForResponseOrThrow();
        crow::json::wvalue result = BuildMovementJson(responsePacket);
        delete responsePacket;

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

crow::response CrowRoutes::HandleRestart(const crow::request& req)
{
    try
    {
        const char* robotIdRaw = req.url_params.get("robotId");

        if (robotIdRaw != nullptr)
        {
            int robotId = std::stoi(robotIdRaw);

            if (!ConsoleUtils::validateRobotId(robotId))
            {
                crow::json::wvalue err;
                err["ack"] = 0;
                err["body"] = "robotId must be 0 or 1";
                return JsonResponse(400, err);
            }
        }

        App* app = GetAppInstance();
        if (app == nullptr)
        {
            crow::json::wvalue err;
            err["ack"] = 0;
            err["body"] = "App instance is not running";
            return JsonResponse(500, err);
        }

        restartAppInstance();

        crow::json::wvalue ok;
        ok["ack"] = 1;
        ok["body"] = "Restart command accepted";

        return JsonResponse(200, ok);
    }
    catch (const std::exception& ex)
    {
        crow::json::wvalue err;
        err["ack"] = 0;
        err["body"] = std::string(ex.what());
        return JsonResponse(500, err);
    }
}

void CrowRoutes::PublishResponse(PktDef* pkt)
{
    std::lock_guard<std::mutex> lock(g_responseMutex);

    if (g_responsePacket != nullptr)
    {
        delete g_responsePacket;
        g_responsePacket = nullptr;
    }

    g_responsePacket = CloneResponsePacket(pkt);
    g_responseReady = (g_responsePacket != nullptr);
    g_responseCv.notify_one();
}

void CrowRoutes::ResetResponseState()
{
    std::lock_guard<std::mutex> lock(g_responseMutex);

    if (g_responsePacket != nullptr)
    {
        delete g_responsePacket;
        g_responsePacket = nullptr;
    }

    g_responseReady = false;
}

