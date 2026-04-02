#include "initializeCrowRoutes.h"

#include <mutex>
#include <thread>
#include <condition_variable>
#include <sstream>
#include <string>
#include <cstring>
#include <chrono>
#include <stdexcept>

namespace
{
    std::once_flag g_workerStartFlag;

    std::mutex g_requestMutex;          // only allow one in-flight REST request at a time
    std::mutex g_responseMutex;
    std::condition_variable g_responseCv;

    PktDef* g_responsePacket = nullptr;
    bool g_responseReady = false;

    int parseRequiredInt(const crow::request& req, const char* key)
    {
        const char* raw = req.url_params.get(key);
        if (raw == nullptr)
        {
            throw std::runtime_error(std::string("Missing query parameter: ") + key);
        }

        return std::stoi(raw);
    }

    int parseOptionalInt(const crow::request& req, const char* key, int defaultValue)
    {
        const char* raw = req.url_params.get(key);
        if (raw == nullptr)
        {
            return defaultValue;
        }

        return std::stoi(raw);
    }

    void validateRobotId(int robotId)
    {
        if (robotId < 0 || robotId > 1)
        {
            throw std::runtime_error("robotId must be 0 or 1");
        }
    }

    void validateDirection(int direction)
    {
        if (direction < 1 || direction > 4)
        {
            throw std::runtime_error("direction must be 1, 2, 3, or 4");
        }
    }

    void validateDuration(int duration)
    {
        if (duration < 1 || duration > 100)
        {
            throw std::runtime_error("duration must be between 1 and 100");
        }
    }

    void validatePower(int power)
    {
        if (power < 80 || power > 100)
        {
            throw std::runtime_error("power must be between 80 and 100");
        }
    }

    crow::response jsonResponse(int statusCode, const crow::json::wvalue& body)
    {
        crow::response res;
        res.code = statusCode;
        res.add_header("Content-Type", "application/json; charset=utf-8");
        res.add_header("Cache-Control", "no-store");
        res.write(body.dump());
        return res;
    }

    crow::response textResponse(int statusCode, const std::string& body, const std::string& contentType)
    {
        crow::response res;
        res.code = statusCode;
        res.add_header("Content-Type", contentType);
        res.add_header("Cache-Control", "no-store");
        res.write(body);
        return res;
    }

    char* buildTelemetryRequestPacket()
    {
        HeaderDef header;
        header.PktCount = 1;
        header.Drive = 0;
        header.Sleep = 0;
        header.Ack = 0;
        header.Status = 1;   // best effort: request telemetry using status flag
        header.Padding = 0;
        header.Length = HeaderDef::HEADER_SIZE + 0 + PktDef::CRCSIZE;

        char* headerBytes = header.serialize();
        char* raw = new char[header.Length + 1]; // +1 because attachRobotID writes at [length + 1]
        std::memset(raw, 0, header.Length + 1);

        std::memcpy(raw, headerBytes, HeaderDef::HEADER_SIZE);

        PktDef crcHelper;
        raw[header.Length - 1] = static_cast<char>(
            crcHelper.countSetBits(raw, header.Length - PktDef::CRCSIZE)
            );

        delete[] headerBytes;
        return raw;
    }

    PktDef* submitPacketAndWait(App& app, PktDef& outboundPacket, int robotId, int timeoutSeconds = 10)
    {
        std::unique_lock<std::mutex> requestLock(g_requestMutex);

        char* rawBuffer = outboundPacket.GenPacket();
        if (rawBuffer == nullptr)
        {
            throw std::runtime_error("Failed to generate outbound packet");
        }

        App::attachRobotID(robotId, rawBuffer);

        {
            std::lock_guard<std::mutex> lk(g_responseMutex);
            if (g_responsePacket != nullptr)
            {
                delete g_responsePacket;
                g_responsePacket = nullptr;
            }
            g_responseReady = false;
        }

        app.dataBuffer = rawBuffer;
        app.hasDataToSend = true;

        std::unique_lock<std::mutex> responseLock(g_responseMutex);
        bool ok = g_responseCv.wait_for(
            responseLock,
            std::chrono::seconds(timeoutSeconds),
            []() { return g_responseReady; }
        );

        if (!ok || g_responsePacket == nullptr)
        {
            throw std::runtime_error("Timed out waiting for robot response");
        }

        PktDef* result = g_responsePacket;
        g_responsePacket = nullptr;
        g_responseReady = false;
        return result;
    }

    PktDef* submitRawPacketAndWait(App& app, char* rawBuffer, int robotId, int timeoutSeconds = 10)
    {
        std::unique_lock<std::mutex> requestLock(g_requestMutex);

        App::attachRobotID(robotId, rawBuffer);

        {
            std::lock_guard<std::mutex> lk(g_responseMutex);
            if (g_responsePacket != nullptr)
            {
                delete g_responsePacket;
                g_responsePacket = nullptr;
            }
            g_responseReady = false;
        }

        app.dataBuffer = rawBuffer;
        app.hasDataToSend = true;

        std::unique_lock<std::mutex> responseLock(g_responseMutex);
        bool ok = g_responseCv.wait_for(
            responseLock,
            std::chrono::seconds(timeoutSeconds),
            []() { return g_responseReady; }
        );

        if (!ok || g_responsePacket == nullptr)
        {
            delete[] rawBuffer;
            throw std::runtime_error("Timed out waiting for robot response");
        }

        PktDef* result = g_responsePacket;
        g_responsePacket = nullptr;
        g_responseReady = false;

        delete[] rawBuffer;
        return result;
    }

    crow::json::wvalue buildMovementJson(PktDef* responsePacket)
    {
        crow::json::wvalue body;
        body["ack"] = responsePacket->GetAck() ? 1 : 0;

        const int bodySize = responsePacket->GetLength() - PktDef::HEADERSIZE - PktDef::CRCSIZE;

        if (bodySize > 0)
        {
            char* serializedBody = responsePacket->GetBodyData();

            if (serializedBody != nullptr)
            {
                // For movement responses this is expected to be AckBodyDef text.
                body["body"] = std::string(serializedBody);
                delete[] serializedBody;
            }
            else
            {
                body["body"] = "";
            }
        }
        else
        {
            body["body"] = "";
        }

        return body;
    }

    crow::json::wvalue buildTelemetryJson(PktDef* responsePacket)
    {
        crow::json::wvalue body;
        body["ack"] = responsePacket->GetAck() ? 1 : 0;

        const int bodySize = responsePacket->GetLength() - PktDef::HEADERSIZE - PktDef::CRCSIZE;
        if (bodySize <= 0)
        {
            return body;
        }

        char* serializedBody = responsePacket->GetBodyData();
        if (serializedBody == nullptr)
        {
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

void initCrow::startAppWorker(App& app)
{
    std::call_once(g_workerStartFlag, [&app]()
        {
            std::thread([&app]()
                {
                    while (true)
                    {
                        // Existing listener path for inbound tunnel traffic.
                        app.openListeners();

                        // REST -> App bridge path.
                        if (app.hasDataToSend)
                        {
                            PktDef* pktInstance = app.sendData(app.dataBuffer, false);

                            {
                                std::lock_guard<std::mutex> lk(g_responseMutex);
                                g_responsePacket = pktInstance;
                                g_responseReady = true;
                                app.hasDataToSend = false;
                            }

                            g_responseCv.notify_one();
                        }
                    }
                }).detach();
        });
}

void initCrow::initializeCrowRoutes(crow::SimpleApp& web, App& app)
{
    startAppWorker(app);

    CROW_ROUTE(web, "/")
        ([]
            {
                fileFetcher fetcher;
                std::string html = fetcher.sendFile("index.html");

                if (html == "not found")
                {
                    return textResponse(404, "index.html not found", "text/plain; charset=utf-8");
                }

                return textResponse(200, html, "text/html; charset=utf-8");
            });

    //CROW_ROUTE(web, "/commands/Drive/<int>")
    //    .methods(crow::HTTPMethod::Get)
    //    ([&app](const crow::request& req, int direction)
    //        {
    //            try
    //            {
    //                validateDirection(direction);

    //                const int robotId = parseRequiredInt(req, "robotId");
    //                validateRobotId(robotId);

    //                const int duration = parseRequiredInt(req, "duration");
    //                validateDuration(duration);

    //                PktDef outbound;
    //                outbound.SetCmd(PktDef::DRIVE);

    //                if (direction == PktDef::FORWARD || direction == PktDef::BACKWARD)
    //                {
    //                    const int power = parseRequiredInt(req, "power");
    //                    validatePower(power);

    //                    DriveBodyDef* body = new DriveBodyDef();
    //                    body->Direction = direction;
    //                    body->Duration = duration;
    //                    body->Power = power;

    //                    outbound.SetBodyData(body);
    //                }
    //                else
    //                {
    //                    TurnBodyDef* body = new TurnBodyDef();
    //                    body->Direction = static_cast<uint8_t>(direction);
    //                    body->Duration = static_cast<uint16_t>(duration);

    //                    outbound.SetBodyData(body);
    //                }

    //                PktDef* responsePacket = submitPacketAndWait(app, outbound, robotId);
    //                crow::json::wvalue result = buildMovementJson(responsePacket);
    //                delete responsePacket;

    //                return jsonResponse(200, result);
    //            }
    //            catch (const std::exception& ex)
    //            {
    //                crow::json::wvalue err;
    //                err["ack"] = 0;
    //                err["body"] = std::string(ex.what());
    //                return jsonResponse(400, err);
    //            }
    //        });

    CROW_ROUTE(web, "/commands/Drive/<int>")
        .methods(crow::HTTPMethod::Get)
        ([&app](const crow::request& req, int direction)
            {
                try
                {
                    const char* robotIdRaw = req.url_params.get("robotId");
                    const char* durationRaw = req.url_params.get("duration");
                    const char* powerRaw = req.url_params.get("power");

                    if (robotIdRaw == nullptr || durationRaw == nullptr) {
                        crow::json::wvalue err;
                        err["ack"] = 0;
                        err["body"] = "Missing robotId or duration";
                        crow::response res;
                        res.code = 400;
                        res.add_header("Content-Type", "application/json; charset=utf-8");
                        res.write(err.dump());
                        return res;
                    }

                    int robotId = std::stoi(robotIdRaw);
                    int duration = std::stoi(durationRaw);

                    if (robotId < 0 || robotId > 1) {
                        crow::json::wvalue err;
                        err["ack"] = 0;
                        err["body"] = "robotId must be 0 or 1";
                        crow::response res;
                        res.code = 400;
                        res.add_header("Content-Type", "application/json; charset=utf-8");
                        res.write(err.dump());
                        return res;
                    }

                    if (direction < 1 || direction > 4) {
                        crow::json::wvalue err;
                        err["ack"] = 0;
                        err["body"] = "direction must be 1-4";
                        crow::response res;
                        res.code = 400;
                        res.add_header("Content-Type", "application/json; charset=utf-8");
                        res.write(err.dump());
                        return res;
                    }

                    PktDef outbound;
                    outbound.SetCmd(PktDef::DRIVE);

                    if (direction == 1 || direction == 2)
                    {
                        if (powerRaw == nullptr) {
                            crow::json::wvalue err;
                            err["ack"] = 0;
                            err["body"] = "power is required for Forward/Backward";
                            crow::response res;
                            res.code = 400;
                            res.add_header("Content-Type", "application/json; charset=utf-8");
                            res.write(err.dump());
                            return res;
                        }

                        int power = std::stoi(powerRaw);

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

                    char* raw = outbound.GenPacket();
                    App::attachRobotID(robotId, raw);

                    std::cout << "=== MOCK DRIVE TEST ===" << std::endl;
                    std::cout << "Direction: " << direction << std::endl;
                    std::cout << "Robot ID: " << robotId << std::endl;
                    std::cout << "Packet Length: " << outbound.GetLength() << std::endl;
                    std::cout << "Raw Packet Bytes: ";
                    BitUtils::displayChar(outbound.GetLength() + 1, raw);

                    crow::json::wvalue ok;
                    ok["ack"] = 1;
                    ok["body"] = "Mock drive packet built successfully";

                    crow::response res;
                    res.code = 200;
                    res.add_header("Content-Type", "application/json; charset=utf-8");
                    res.write(ok.dump());
                    return res;
                }
                catch (const std::exception& ex)
                {
                    crow::json::wvalue err;
                    err["ack"] = 0;
                    err["body"] = ex.what();

                    crow::response res;
                    res.code = 400;
                    res.add_header("Content-Type", "application/json; charset=utf-8");
                    res.write(err.dump());
                    return res;
                }
            });

    // Optional but useful because your UI already has a sleep button.
    CROW_ROUTE(web, "/commands/sleep")
        .methods(crow::HTTPMethod::Get)
        ([&app](const crow::request& req)
            {
                try
                {
                    const int robotId = parseRequiredInt(req, "robotId");
                    validateRobotId(robotId);

                    PktDef outbound;
                    outbound.SetCmd(PktDef::SLEEP);
                    outbound.SetBodyData(nullptr);

                    PktDef* responsePacket = submitPacketAndWait(app, outbound, robotId);
                    crow::json::wvalue result = buildMovementJson(responsePacket);
                    delete responsePacket;

                    return jsonResponse(200, result);
                }
                catch (const std::exception& ex)
                {
                    crow::json::wvalue err;
                    err["ack"] = 0;
                    err["body"] = std::string(ex.what());
                    return jsonResponse(400, err);
                }
            });

    CROW_ROUTE(web, "/commands/telem")
        .methods(crow::HTTPMethod::Get)
        ([&app](const crow::request& req)
            {
                try
                {
                    const int robotId = parseRequiredInt(req, "robotId");
                    validateRobotId(robotId);

                    char* rawTelemRequest = buildTelemetryRequestPacket();

                    PktDef* responsePacket = submitRawPacketAndWait(app, rawTelemRequest, robotId);
                    crow::json::wvalue result = buildTelemetryJson(responsePacket);
                    delete responsePacket;

                    return jsonResponse(200, result);
                }
                catch (const std::exception& ex)
                {
                    crow::json::wvalue err;
                    err["ack"] = 0;
                    err["error"] = std::string(ex.what());
                    return jsonResponse(400, err);
                }
            });
}