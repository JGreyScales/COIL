#include <iostream>
#include <bitset>
#include <limits>
#include <thread>

#define NOMINMAX

#include "App.h"
#include "fileFetcher.h"


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

	std::cout << "Enter TCP (1) / UDP (2)" << std::endl;
	MySocket::ConnectionType connectionType = (MySocket::ConnectionType)ConsoleUtils::getUserInt(1, 2);

	std::cout << "Enter IP: ";
	std::string IP = ConsoleUtils::getUserString();

	std::cout << "Enter Port: ";
	unsigned int port = ConsoleUtils::getUserInt(0, 65535); // Ports only go up to 65535

	MySocket* socket = new MySocket(MySocket::CLIENT, IP, port, connectionType, MySocket::DEFAULT_SIZE);
	if (connectionType == MySocket::ConnectionType::TCP) {
		socket->ConnectTCP();
	}

	std::cout << "Sending to a server? Y(1) N(2)" << std::endl;
	bool attachRobotID = 1 == ConsoleUtils::getUserInt(1, 2);

	while (interfaceWithUser) {
		std::cout << "Select robot, or 3 for exit" << std::endl;
		int chosenInput = ConsoleUtils::getUserInt(1, 3) - 1;

		if (chosenInput == 2) {
			interfaceWithUser = false;
			continue;
		}
		Robot* chosenRobot = robots[chosenInput];



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

				if (attachRobotID) {
					App::attachRobotID(chosenRobot->id, packetData);
				}

				socket->SendData(packetData, pktInstance->GetLength() + 1);



				int bytesWritten = socket->GetData(recieveBuffer);
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
				if (socket) {
					std::cout << "Sending: " << pktInstance->GetLength() << " bytes" << std::endl;
					socket->SendData(packetData, pktInstance->GetLength());
				}

				// do we care about the ack here?
				// I guess so.....

				int bytesWritten = socket->GetData(recieveBuffer);
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
	crow::SimpleApp crowApp;
	fileFetcher ff;

	// GET "/" - serve root HTML page
	CROW_ROUTE(crowApp, "/")
		([&ff]() {
		std::string html = ff.sendFile("src/index.html");
		if (html == "not found") {
			return crow::response(404, "Not Found");
		}
		crow::response res(200, html);
		res.set_header("Content-Type", "text/html");
		return res;
			});

	// POST /commands/Drive/<int> - for drive & turn
	CROW_ROUTE(crowApp, "/commands/Drive/<int>")
		.methods("POST"_method)
		([](const crow::request& req, int direction) {
		auto body = crow::json::load(req.body);
		if (!body) {
			return crow::response(400, "Invalid JSON");
		}

		crow::json::wvalue response;

		if (direction == 1 || direction == 2) {
			// Drive forward/backward - needs Duration and Power
			if (!body.has("Duration") || !body.has("Power")) {
				return crow::response(400, "Duration and Power required");
			}
			int duration = body["Duration"].i();
			int power = body["Power"].i();


			response["Ack_Flag"] = 1;
			response["body"] = "Drive command received";
		}
		else if (direction == 3 || direction == 4) {
			// Turn right/left - needs Duration
			if (!body.has("Duration")) {
				return crow::response(400, "Duration required");
			}
			int duration = body["Duration"].i();

			response["Ack_Flag"] = 1;
			response["body"] = "Turn command received";
		}
		else {
			return crow::response(404, "Invalid direction");
		}

		return crow::response(response);
			});

	// GET /commands/telemetry
	CROW_ROUTE(crowApp, "/commands/telemetry")
		.methods("GET"_method)
		([]() {
		crow::json::wvalue response;
		// TODO: integrate with App to get actual telemetry
		response["Ack"] = 1;
		return crow::response(response);
			});

	// POST /commands/sleep
	CROW_ROUTE(crowApp, "/commands/sleep")
		.methods("POST"_method)
		([]() {
		// TODO: integrate with App to send sleep command
		return crow::response(200, "sleep");
			});


	std::cout << "Starting crow server" << std::endl;
	std::thread crowThread([&crowApp]() {
		crowApp.loglevel(crow::LogLevel::Warning);
		crowApp.port(8080).multithreaded().run();
		});
	crowThread.detach();



	while (1) {
		std::cout << "Manual input(1) or webserver(2)" << std::endl;
		int input = ConsoleUtils::getUserInt(1, 2);

		if (input == 1) {
			manualInputMode();
		}
		else {
			// if app already exists
			// start app instance will delete it and start a new instance
			startAppInstance();
			App* app = GetAppInstance();

			std::cout << "app is running" << std::endl;

			while (!(app->restartCommand)) {
				// should hold for 100ms per cycle to check for inbound data
				// if inbound data it will daisy chain it and process it all before returning
				// if not inbound data, it will return and proceed with the code
				app->openListeners();

				if (app->hasDataToSend) {
					PktDef* pktInstance = app->sendData(app->dataBuffer, false);


					// put logic in here to get it back into the rest route
					pktInstance->display();

					delete pktInstance;
					app->hasDataToSend = false;
				}
			}
		}
	}

#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}