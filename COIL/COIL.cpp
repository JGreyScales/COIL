// COIL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <bitset>
#include <limits>

#include <winsock2.h>

#include "Utils.h"

#include "Robot.h"

int main()
{
  
  WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed." << std::endl;
		return 1;
	}
  
	Robot* robots[2] = { new Robot(0), new Robot(1) };
	bool interfaceWithUser = true;
	
    while (interfaceWithUser) {
        std::cout << "Select robot, or 3 for exit" << std::endl;
        int chosenInput = ConsoleUtils::getUserInt(1, 3) - 1;
        // 2 because we subtract one for user ease on the display
        if (chosenInput == 2) {
            interfaceWithUser = false;
        }
        Robot* chosenRobot = robots[chosenInput];

        std::cout << "Enter TCP (1) / UDP (2)" << std::endl;
        // cast the int into the connection type
        MySocket::ConnectionType connectionType = (MySocket::ConnectionType)ConsoleUtils::getUserInt(1, 2);

        std::string IP = ConsoleUtils::getUserString();
        unsigned int port = ConsoleUtils::getUserInt(0, INT_MAX);

        chosenRobot->socket = new MySocket(MySocket::SERVER, IP, port, connectionType, MySocket::DEFAULT_SIZE);
        bool robotInterfacing = true;

        // controlling the actual robot
        while (robotInterfacing) {

            std::cout << "Select an option: Drive(0) Sleep(1) Exit(2) ChangeRobot(3)" << std::endl;
            chosenInput = ConsoleUtils::getUserInt(0, 3);

            switch (chosenInput)
            {
            case 0: // Drive/Turn Option
            {
                std::cout << "Select a direction: forwards(1), backwards(2), right(3), left(4)" << std::endl;
                int moveInput = ConsoleUtils::getUserInt(1, 4);

                // Create the packet container
                PktDef* pktInstance = new PktDef();
                pktInstance->SetCmd(PktDef::DRIVE);

                switch (moveInput)
                {
                case 1:
                case 2:
                {
                    DriveBodyDef* driveInstance = new DriveBodyDef();
                    driveInstance->Direction = moveInput;

                    std::cout << "Duration(s): 1-100" << std::endl;
                    driveInstance->Duration = ConsoleUtils::getUserInt(1, 100);

                    std::cout << "Power: 1-100" << std::endl;
                    driveInstance->Power = ConsoleUtils::getUserInt(1, 100);

                    pktInstance->SetBodyData(driveInstance);
                    break;
                }
                case 3:
                case 4:
                {
                    TurnBodyDef* turnInstance = new TurnBodyDef();
                    turnInstance->Direction = moveInput;

                    std::cout << "Duration(s): 1-100" << std::endl;
                    turnInstance->Duration = ConsoleUtils::getUserInt(1, 100);

                    pktInstance->SetBodyData(turnInstance);
                    break;
                }
                default:
                    break;
                }

                char* packetData = pktInstance->GenPacket();
                chosenRobot->socket->SendData(packetData, pktInstance->GetLength());

                delete pktInstance;
                break;
            }
            case 1: // Sleep Option
            {
                PktDef* pktInstance = new PktDef();
                pktInstance->SetCmd(PktDef::SLEEP);

                char* packetData = pktInstance->GenPacket();
                chosenRobot->socket->SendData(packetData, pktInstance->GetLength());

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
            default:
                std::cout << "Invalid selection" << std::endl;
                break;
            }
        }
    }

	delete robots[0];
	delete robots[1];
  WSACleanup();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
