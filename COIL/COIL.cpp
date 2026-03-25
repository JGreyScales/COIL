// COIL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <bitset>
#include <limits>

#include "Utils.h"

#include "PktDef.h"
#include "Robot.h"

int main()
{
	Robot* robots[2] = { new Robot(0), new Robot(1) };
	bool interfaceWithUser = true;
	
	while (interfaceWithUser) {
		std::cout << "Select robot" << std::endl;
		Robot* chosenRobot = robots[ConsoleUtils::getUserInt(1, 2) - 1];

		std::cout << "Enter TCP (1) / UDP (2)" << std::endl;
		// cast the int into the connection type
		MySocket::ConnectionType connectionType = (MySocket::ConnectionType)ConsoleUtils::getUserInt(1, 2);

		std::string IP = ConsoleUtils::getUserString();
		unsigned int port = ConsoleUtils::getUserInt(0, INT_MAX);

		chosenRobot->socket = new MySocket(MySocket::SERVER, IP, port, connectionType, MySocket::DEFAULT_SIZE);
		bool robotInterfacing = true;

		// controlling the actual robot
		while (robotInterfacing) {
			int chosenInput;
			
			std::cout << "Select an option: Drive(0) Sleep(1) Exit(2) ChangeRobot(3)" << std::endl;
			chosenInput = ConsoleUtils::getUserInt(0, 3);

			switch (chosenInput)
			{
			case 0:
				std::cout << "Select a direction: forwards(1), backwards(2), right(3), left(4)" << std::endl;
				chosenInput = ConsoleUtils::getUserInt(1, 4);
				switch (chosenInput)
				{
				case 1:
				case 2:
					PktDef * pktInstance = new PktDef();
					pktInstance->SetCmd(PktDef::DRIVE);

					DriveBodyDef * driveInstance = new DriveBodyDef();
					driveInstance->Direction = chosenInput;

					std::cout << "Duration(s): 1-100" << std::endl;
					driveInstance->Duration = ConsoleUtils::getUserInt(1, 100);

					std::cout << "Power: 1-100" << std::endl;
					driveInstance->Power = ConsoleUtils::getUserInt(1, 100);

					pktInstance->SetBodyData(driveInstance);
					char* packetData = pktInstance->GenPacket();

					chosenRobot->socket->SendData(packetData, pktInstance->GetLength());
					break;
				case 3:
				case 4:
					PktDef * pktInstance = new PktDef();
					pktInstance->SetCmd(PktDef::DRIVE);

					TurnBodyDef* turnInstance = new TurnBodyDef();
					turnInstance->Direction = chosenInput;

					std::cout << "Duration(s): 1-100" << std::endl;
					turnInstance->Duration = ConsoleUtils::getUserInt(1, 100);

					pktInstance->SetBodyData(turnInstance);
					char* packetData = pktInstance->GenPacket();

					chosenRobot->socket->SendData(packetData, pktInstance->GetLength());
					break;

				default:
					break;
				}
				
				break;
			case 1:
				break;
			case 2:
				break;
			case 3: 
				break;
			default:
				std::cout << "Invalid selection" << std::endl;
				break;
			}

			if (chosenInput == 0) {

			}

		}
	}

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
