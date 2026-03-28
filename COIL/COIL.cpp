#include <iostream>
#include <bitset>
#include <limits>
#include "Utils.h"

#include "fileFetcher.h"

#include "PktDef.h"
#include "Robot.h"


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

int main()
{
    test();
    // --- Windows Socket Startup ---
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }
#endif

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

        // Create the socket
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
                chosenRobot->socket->SendData(packetData, pktInstance->GetLength());
                 

                // wait for ack reply
                int bytesWritten = chosenRobot->socket->GetData(recieveBuffer);
                std::cout << bytesWritten << " Bytes written" << std::endl;

                BitUtils::displayChar(bytesWritten, recieveBuffer);
                if (bytesWritten > 0) {
                    PktDef* ackPacket = new PktDef(recieveBuffer);
                    ackPacket->display();
                }

                std::memset(recieveBuffer, 0, MySocket::DEFAULT_SIZE);

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

    // --- Windows Socket Cleanup ---
#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}