#include <iostream>
#include <bitset>
#include <limits>

#define ASIO_STANDALONE
#include "crow_all.h"
#include "fileFetcher.h"
#include "App.h"


// old stuff, wouldnt be included in release
void test() {
    PktDef* pktInstance = new PktDef();
    pktInstance->SetCmd(PktDef::DRIVE);

    DriveBodyDef* driveInstance = new DriveBodyDef();
    driveInstance->Duration = 20;
    driveInstance->Power = 50;
    driveInstance->Direction = 40;

    pktInstance->SetBodyData(driveInstance);
    pktInstance->GenPacket();
    pktInstance->display();

    delete pktInstance;
}

// old stuff, wouldnt be included in release
void manualInputMode() {
    Robot* robots[2] = { new Robot(0), new Robot(1) };
    bool interfaceWithUser = true;
    char recieveBuffer[MySocket::DEFAULT_SIZE];


    while (interfaceWithUser) {
        std::cout << "Select robot, or 3 for exit" << std::endl;
        int chosenInput = ConsoleUtils::getUserInt(1, 3) - 1;

        if (chosenInput == 2) {
            interfaceWithUser = false;
            continue;
        }
        Robot* chosenRobot = robots[chosenInput];

        std::cout << "Enter TCP (1) / UDP (2)" << std::endl;
        MySocket::ConnectionType connectionType = (MySocket::ConnectionType)ConsoleUtils::getUserInt(1, 2);

        std::cout << "Enter IP: ";
        std::string IP = ConsoleUtils::getUserString();

        std::cout << "Enter Port: ";
        unsigned int port = ConsoleUtils::getUserInt(0, 65535); // Ports only go up to 65535

        chosenRobot->socket = new MySocket(MySocket::CLIENT, IP, port, connectionType, MySocket::DEFAULT_SIZE);
        if (connectionType == MySocket::ConnectionType::TCP) {
            chosenRobot->socket->ConnectTCP();
        }

        bool robotInterfacing = true;

        while (robotInterfacing) {
            std::cout << "Select an option: Drive(0) Sleep(1) Exit(2) ChangeRobot(3)" << std::endl;
            chosenInput = ConsoleUtils::getUserInt(0, 3);

            switch (chosenInput)
            {
            case 0: // Drive/Turn Option
            {
                std::cout << "Select a direction: forwards(1), backwards(2), right(3), left(4)" << std::endl;
                int moveInput = ConsoleUtils::getUserInt(1, 4);

                PktDef* pktInstance = new PktDef();
                pktInstance->SetCmd(PktDef::DRIVE);

                if (moveInput == 1 || moveInput == 2) {
                    DriveBodyDef* driveInstance = new DriveBodyDef();
                    driveInstance->Direction = moveInput;
                    std::cout << "Duration(s): 1-100" << std::endl;
                    driveInstance->Duration = ConsoleUtils::getUserInt(1, 100);
                    std::cout << "Power: 80-100" << std::endl;
                    driveInstance->Power = ConsoleUtils::getUserInt(80, 100);
                    pktInstance->SetBodyData(driveInstance);
                }
                else {
                    TurnBodyDef* turnInstance = new TurnBodyDef();
                    turnInstance->Direction = moveInput;
                    std::cout << "Duration(s): 1-100" << std::endl;
                    turnInstance->Duration = ConsoleUtils::getUserInt(1, 100);
                    pktInstance->SetBodyData(turnInstance);
                }

                char* packetData = pktInstance->GenPacket();
                BitUtils::displayChar(pktInstance->GetLength(), packetData);
                std::cout << "Sending: " << pktInstance->GetLength() << " bytes" << std::endl;
                
                App::attachRobotID(0, packetData);
                chosenRobot->socket->SendData(packetData, pktInstance->GetLength() + 1);



                int bytesWritten = chosenRobot->socket->GetData(recieveBuffer);
                std::cout << bytesWritten << " Bytes written" << std::endl;

                BitUtils::displayChar(bytesWritten, recieveBuffer);
                if (bytesWritten > 0) {
                    PktDef* ackPacket = new PktDef(recieveBuffer);
                    ackPacket->display();
                }

                std::memset(recieveBuffer, 0, MySocket::DEFAULT_SIZE);

                pktInstance->deleteBuffer = false;
                delete pktInstance;
                break;
            }
            case 1: // Sleep Option
            {
                PktDef* pktInstance = new PktDef();
                pktInstance->SetCmd(PktDef::SLEEP);

                char* packetData = pktInstance->GenPacket();
                if (chosenRobot->socket) {
                    std::cout << "Sending: " << pktInstance->GetLength() << " bytes" << std::endl;
                    chosenRobot->socket->SendData(packetData, pktInstance->GetLength());
                }

                // do we care about the ack here?
                // I guess so.....

                int bytesWritten = chosenRobot->socket->GetData(recieveBuffer);
                std::cout << bytesWritten << " Bytes written" << std::endl;
                if (bytesWritten > 0) {
                    PktDef* ackPacket = new PktDef(recieveBuffer);
                    ackPacket->display();
                }
                std::memset(recieveBuffer, 0, MySocket::DEFAULT_SIZE);


                delete pktInstance;
                break;
            }
            case 2: // Reset Robot Option
            {
                if (robots[0] == chosenRobot) {
                    delete robots[0];
                    robots[0] = new Robot(0);
                }
                else {
                    delete robots[1];
                    robots[1] = new Robot(1);
                }
                robotInterfacing = false;
                break;
            }
            case 3: // Change Robot Option
                robotInterfacing = false;
                break;
            }
        }
    }

    delete robots[0];
    delete robots[1];
}



int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }
#endif

    std::cout << "Manual input(1) or webserver(2)" << std::endl;
    int input = ConsoleUtils::getUserInt(1, 2);

    if (input == 1) {
        manualInputMode();
    }
    else {

        crow::SimpleApp crowApp;
        fileFetcher fetcher;

        // Robot connection state
        static MySocket* robotSocket = nullptr;
        static std::string robotIP;
        static int robotPort = 0;

        // Helper: build hex string from buffer
        auto toHex = [](const char* buf, int len) {
            std::string hex;
            for (int i = 0; i < len; i++) {
                char h[4];
                snprintf(h, sizeof(h), "%02X ", (unsigned char)buf[i]);
                hex += h;
            }
            return hex;
        };

        // GET / - serve the main GUI
        CROW_ROUTE(crowApp, "/")([
            &fetcher]() {
                return fetcher.sendFile("index.html", "src");
            });

        // Serve static files
        CROW_ROUTE(crowApp, "/styles/<string>")([
            &fetcher](std::string filename) {
                return fetcher.sendFile(filename, "styles");
            });

        CROW_ROUTE(crowApp, "/scripts/<string>")([
            &fetcher](std::string filename) {
                return fetcher.sendFile(filename, "scripts");
            });

        // POST /connect/<string>/<int> - set robot IP and port for UDP comms
        CROW_ROUTE(crowApp, "/connect/<string>/<int>").methods("POST"_method)([
            &toHex](std::string ip, int port) {
                robotIP = ip;
                robotPort = port;

                // Clean up existing socket
                if (robotSocket) {
                    delete robotSocket;
                    robotSocket = nullptr;
                }

                robotSocket = new MySocket(MySocket::CLIENT, ip, port, MySocket::UDP, MySocket::DEFAULT_SIZE);

                std::cout << "[CONNECT] Robot IP: " << ip << " Port: " << port << std::endl;

                crow::json::wvalue resp;
                resp["status"] = "ok";
                resp["ip"] = ip;
                resp["port"] = port;
                return crow::response(200, resp);
            });

        // PUT /telecommand/ - send Drive or Sleep command
        CROW_ROUTE(crowApp, "/telecommand/").methods("PUT"_method)([
            &toHex](const crow::request& req) {
                if (!robotSocket) {
                    return crow::response(400, "Not connected. Use /connect first.");
                }

                auto body = crow::json::load(req.body);
                if (!body) return crow::response(400, "Invalid JSON");

                std::string command = body["command"].s();
                crow::json::wvalue resp;

                PktDef* pkt = new PktDef();

                if (command == "SLEEP") {
                    pkt->SetCmd(PktDef::SLEEP);
                    char* packetData = pkt->GenPacket();
                    int length = pkt->GetLength();

                    std::memset(App::rawBuffer, 0, App::RAW_BUFFER_SIZE);
                    std::memcpy(App::rawBuffer, packetData, length);
                    App::rawBufferLength = length;

                    std::cout << "[TX] SLEEP | " << length << " bytes | " << toHex(App::rawBuffer, length) << std::endl;

                    robotSocket->SendData(App::rawBuffer, length);

                    // Await ACK
                    char recvBuf[MySocket::DEFAULT_SIZE] = { 0 };
                    int bytesRecv = robotSocket->GetData(recvBuf);

                    resp["status"] = "ok";
                    resp["command"] = "SLEEP";
                    resp["txLength"] = length;
                    resp["txHex"] = toHex(App::rawBuffer, length);

                    if (bytesRecv > 0) {
                        PktDef* ackPkt = new PktDef(recvBuf);
                        resp["ack"] = ackPkt->GetAck() ? "ACK" : "NACK";
                        resp["rxLength"] = bytesRecv;
                        resp["rxHex"] = toHex(recvBuf, bytesRecv);
                        std::cout << "[RX] " << (ackPkt->GetAck() ? "ACK" : "NACK") << " | " << bytesRecv << " bytes" << std::endl;
                        delete ackPkt;
                    } else {
                        resp["ack"] = "NO RESPONSE";
                    }

                } else {
                    // DRIVE or TURN
                    pkt->SetCmd(PktDef::DRIVE);
                    int direction = body["direction"].i();
                    int duration = body["duration"].i();

                    if (direction == PktDef::FORWARD || direction == PktDef::BACKWARD) {
                        int power = body["power"].i();
                        DriveBodyDef* driveBody = new DriveBodyDef();
                        driveBody->Direction = direction;
                        driveBody->Duration = duration;
                        driveBody->Power = power;
                        pkt->SetBodyData(driveBody);
                        resp["power"] = power;
                    } else {
                        TurnBodyDef* turnBody = new TurnBodyDef();
                        turnBody->Direction = direction;
                        turnBody->Duration = duration;
                        pkt->SetBodyData(turnBody);
                    }

                    char* packetData = pkt->GenPacket();
                    int length = pkt->GetLength();

                    std::memset(App::rawBuffer, 0, App::RAW_BUFFER_SIZE);
                    std::memcpy(App::rawBuffer, packetData, length);
                    App::rawBufferLength = length;

                    std::cout << "[TX] DRIVE dir=" << direction << " dur=" << duration << " | "
                              << length << " bytes | " << toHex(App::rawBuffer, length) << std::endl;

                    robotSocket->SendData(App::rawBuffer, length);

                    // Await ACK
                    char recvBuf[MySocket::DEFAULT_SIZE] = { 0 };
                    int bytesRecv = robotSocket->GetData(recvBuf);

                    resp["status"] = "ok";
                    resp["command"] = "DRIVE";
                    resp["direction"] = direction;
                    resp["duration"] = duration;
                    resp["txLength"] = length;
                    resp["txHex"] = toHex(App::rawBuffer, length);

                    if (bytesRecv > 0) {
                        PktDef* ackPkt = new PktDef(recvBuf);
                        resp["ack"] = ackPkt->GetAck() ? "ACK" : "NACK";
                        resp["rxLength"] = bytesRecv;
                        resp["rxHex"] = toHex(recvBuf, bytesRecv);
                        std::cout << "[RX] " << (ackPkt->GetAck() ? "ACK" : "NACK") << " | " << bytesRecv << " bytes" << std::endl;
                        delete ackPkt;
                    } else {
                        resp["ack"] = "NO RESPONSE";
                    }

                    pkt->deleteBuffer = false;
                }

                delete pkt;
                return crow::response(200, resp);
            });

        // GET /telementry_request/ - request housekeeping telemetry
        CROW_ROUTE(crowApp, "/telementry_request/").methods("GET"_method)([
            &toHex]() {
                if (!robotSocket) {
                    return crow::response(400, "Not connected. Use /connect first.");
                }

                PktDef* pkt = new PktDef();
                pkt->SetCmd(PktDef::RESPONSE);

                char* packetData = pkt->GenPacket();
                int length = pkt->GetLength();

                std::memset(App::rawBuffer, 0, App::RAW_BUFFER_SIZE);
                std::memcpy(App::rawBuffer, packetData, length);
                App::rawBufferLength = length;

                std::cout << "[TX] TELEMETRY REQUEST | " << length << " bytes | " << toHex(App::rawBuffer, length) << std::endl;

                robotSocket->SendData(App::rawBuffer, length);

                // Await telemetry response
                char recvBuf[MySocket::DEFAULT_SIZE] = { 0 };
                int bytesRecv = robotSocket->GetData(recvBuf);

                crow::json::wvalue resp;
                resp["status"] = "ok";
                resp["command"] = "TELEMETRY_REQUEST";
                resp["txLength"] = length;
                resp["txHex"] = toHex(App::rawBuffer, length);

                if (bytesRecv > 0) {
                    PktDef* resPkt = new PktDef(recvBuf);
                    resp["rxLength"] = bytesRecv;
                    resp["rxHex"] = toHex(recvBuf, bytesRecv);

                    if (resPkt->GetCmd() == PktDef::RESPONSE && resPkt->getPacket().Header->Status) {
                        TelemetryBodyDef* telemetry = (TelemetryBodyDef*)resPkt->getPacket().body;
                        resp["lastPktCounter"] = telemetry->LastPktCounter;
                        resp["currentGrade"] = telemetry->CurrentGrade;
                        resp["hitCount"] = telemetry->HitCount;
                        resp["heading"] = telemetry->Heading;
                        resp["lastCmd"] = telemetry->LastCmd;
                        resp["lastCmdValue"] = telemetry->LastCmdValue;
                        resp["lastCmdPower"] = telemetry->LastCmdPower;
                        std::cout << "[RX] TELEMETRY | Grade: " << telemetry->CurrentGrade
                                  << " Hits: " << telemetry->HitCount
                                  << " Heading: " << telemetry->Heading << std::endl;
                    } else {
                        resp["ack"] = resPkt->GetAck() ? "ACK" : "NACK";
                    }
                    delete resPkt;
                } else {
                    resp["ack"] = "NO RESPONSE";
                }

                delete pkt;
                return crow::response(200, resp);
            });

        // POST /routing_table/ - route commands to another instance
        CROW_ROUTE(crowApp, "/routing_table/").methods("POST"_method)(
            [](const crow::request& req) {
                auto body = crow::json::load(req.body);
                if (!body) return crow::response(400, "Invalid JSON");

                std::string nextIP = body["ip"].s();
                int nextPort = body["port"].i();

                std::cout << "[ROUTE] Next hop: " << nextIP << ":" << nextPort << std::endl;

                crow::json::wvalue resp;
                resp["status"] = "ok";
                resp["nextHopIP"] = nextIP;
                resp["nextHopPort"] = nextPort;
                return crow::response(200, resp);
            });

        std::cout << "Starting Crow webserver on port 8080..." << std::endl;
        crowApp.port(8080).multithreaded().run();
    }

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}