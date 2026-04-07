#include "App.h"

// how to use
// create your app using new App
// setup your connections
// put open Listners into a while true loop, it will try to read data blocking progress for 100 milliseconds on every call
// put a flag that if there is data to send that it will 
//	attach robot ID to the buffer
//	use sendData


App::App() {
	this->dataBuffer = nullptr;
	this->connectionType = MySocket::ConnectionType::UNDEFINED;

	this->restartCommand = false;
	this->hasDataToSend = false;
	this->networkLock = false;

	std::cout << "Primary(1) or Secondary Mode(2)" << std::endl;
	int chosenInput = ConsoleUtils::getUserInt(1, 2);

	if (chosenInput == 1) {
		this->mode = appMode::Primary;
		this->robots[0] = new Robot(0);
		this->robots[1] = new Robot(1);
		this->nexthop = nullptr;

		std::cout << "What is my ethernet binding IP" << std::endl;
		std::string ethernetIP = ConsoleUtils::getUserString();

		for (size_t i = 0; i < 2; i++)
		{
			std::cout << "Enter TCP (1) / UDP (2)" << std::endl;
			MySocket::ConnectionType connectionType = (MySocket::ConnectionType)ConsoleUtils::getUserInt(1, 2);

			std::cout << "Enter IP: ";
			std::string IP = ConsoleUtils::getUserString();
			
			std::cout << "Enter Port: ";
			unsigned int port = ConsoleUtils::getUserInt(0, 65535);


			this->robots[i]->socket = new MySocket(MySocket::CLIENT, IP, port, connectionType, MySocket::DEFAULT_SIZE);
			if (connectionType == MySocket::ConnectionType::TCP) {
				this->robots[i]->socket->BindLocalIP(ethernetIP);
				this->robots[i]->socket->ConnectTCP();
			}
		}

	}
	else {
		this->mode = appMode::Secondary;

		std::cout << "What is my next hops IP:";
		std::string IP = ConsoleUtils::getUserString();

		std::cout << "What is my next hops tunnel port" << std::endl;
		int Port = ConsoleUtils::getUserInt(0, 65535);

		std::cout << "What is my next hop using TCP (1) / UDP (2)" << std::endl;
		MySocket::ConnectionType connectionType = (MySocket::ConnectionType)ConsoleUtils::getUserInt(1, 2);

		// outbound
		this->nexthop = new MySocket(MySocket::CLIENT, IP, Port, connectionType, 1024);
		if (connectionType == MySocket::ConnectionType::TCP) {
			this->nexthop->ConnectTCP();
		}

	}
	std::cout << "Do I host a recieve socket? No(1), Yes(2)" << std::endl;
	if (ConsoleUtils::getUserInt(1, 2) == 2) {
		std::cout << "Am I TCP(1) or UDP(2) or end(3)" << std::endl;
		const int input = ConsoleUtils::getUserInt(1, 3);
		
		if (input == 1) {
			this->connectionType = MySocket::TCP;
		}
		else if (input == 2){
			this->connectionType = MySocket::UDP;
		}
		else {
			this->curSoc = nullptr;
			return;
		}

		std::cout << "What is my wifiIP" << std::endl;
		std::string wifiIP = ConsoleUtils::getUserString();

		std::cout << "What is my tunnel port" << std::endl;
		const int port = ConsoleUtils::getUserInt(0, 65535);

		if (this->connectionType == MySocket::TCP) {
			std::cout << "Waiting for client to connect..." << std::endl;
		}

		// inbound
		this->curSoc = new MySocket(MySocket::SERVER, wifiIP, port, this->connectionType, 1024);
	}
	else {
		this->curSoc = nullptr;
		this->connectionType = MySocket::UNDEFINED;
	}
}


// for both of these functions, the raw buffer
void App::attachRobotID(int ID, char* data)
{
	PktDef* pktInstance = new PktDef(data);
	pktInstance->deleteBuffer = false;

	size_t length = pktInstance->GetLength();

	data[length] = ID;

	delete pktInstance;
}

int App::detachRobotID(char* data)
{
	PktDef* pktInstance = new PktDef(data);
	pktInstance->deleteBuffer = false;

	size_t length = pktInstance->GetLength();

	int ID = data[length];
	std::cout << "Data at robot ID byte " << std::endl;
	BitUtils::displayBin(length + 1, data);

	delete pktInstance;

	return ID;
}

char* App::awaitReturn(MySocket* socket, char* buffer)
{
	std::cout << "awaiting response" << std::endl;
	int bytes = socket->GetData(buffer);

	std::cout << "Recieved: " << bytes << " bytes" << std::endl;
	return buffer;
}

char * App::forwardToNextHop(char* buffer)
{
	PktDef* pktInstance = new PktDef(buffer);
	pktInstance->deleteBuffer = false;

	// + 1 to account for the robot ID
	this->nexthop->SendData(buffer, pktInstance->GetLength() + 1);

	delete pktInstance;


	return this->awaitReturn(this->nexthop, buffer);
}

char * App::sendToRobot(Robot* robot, char* buffer)
{
	PktDef* pktInstance = new PktDef(buffer);
	pktInstance->deleteBuffer = false;

	std::cout << "sending data to robot" << std::endl;

	robot->socket->SendData(buffer, pktInstance->GetLength());

	if (pktInstance->getPacket().Header->Status) {
		char* _ = this->awaitReturn(robot->socket, buffer); // ack response
		delete pktInstance;

		return this->awaitReturn(robot->socket, buffer); // teleresponse
	}
	else {
		delete pktInstance;

		return this->awaitReturn(robot->socket, buffer);
	}
	
}

// attachRobotID must have been called previous to this function call
// or this will not work as intended

// respond is for if a response on the server socket should be sent
// if the request came from this server, it shouldnt be responding
// it should take the PktDef* and convert it over to rest to respond to the webserver
PktDef* App::sendData(char* buffer, bool respond) { // defaults to true
	if (this->networkLock) {
		// if a request is currently being proccessed, dont process the request
		return NULL;
	}
	
	this->lock(0); // lock for 0 seconds for debug

	bool terminateConnection = false;
	char* returnData;

	PktDef* pktToSend = new PktDef(buffer);
	pktToSend->deleteBuffer = false;

	if (pktToSend->cmd == PktDef::SLEEP) {
		terminateConnection = true;
	}

	delete pktToSend;

	// move down the daisy chain
	if (this->mode == appMode::Primary) {
		std::cout << "sending to server" << std::endl;
		int robotID = this->detachRobotID(buffer);
		std::cout << "Sending to robot: " << robotID << std::endl;
		returnData = this->sendToRobot(this->robots[robotID], buffer);
		

		// inside disconnect, it can catch if its UDP and handle it
		if (terminateConnection) {
			this->robots[0]->socket->DisconnectTCP();
			this->robots[1]->socket->DisconnectTCP();
		}
	}
	else {
		std::cout << "sending to next hop" << std::endl;
		returnData = this->forwardToNextHop(buffer);
	}


	PktDef* pktInstance = new PktDef(returnData);

	if (this->curSoc != nullptr && respond) {
		std::cout << "responding on socket" << std::endl;
		this->curSoc->SendData(returnData, pktInstance->GetLength()); // move up the daisy chain 
	}

	if (terminateConnection) {
		std::cout << "terminating connection" << std::endl;
		if (this->curSoc != nullptr) {
			this->curSoc->DisconnectTCP(); // this has a catch for if its udp
			// no reason to catch that here
		}
		App* app = GetAppInstance();
		app->restartCommand = true;
	}

	return pktInstance;
}

// checks for 100 milliseconds if there is data waiting to be read
bool App::openListeners()	
{
	char buffer[1024];
	if (this->curSoc != nullptr) {
		if (this->curSoc->HasData(100)) {
			int bytes = this->curSoc->GetData(buffer);
			PktDef* pktInstance = this->sendData(buffer);
			if (pktInstance != NULL) {
				delete pktInstance; // we dont need to hold onto the packet
			}
		}
	}


	return true;
}

void App::lock(int duration)
{
	this->networkLock = true;

	// lol this isnt thread safe
	// but thats okay
	std::thread([this, duration]() {
		// Put the background thread to sleep
		std::this_thread::sleep_for(std::chrono::seconds(duration));

		// Unlock after the timer expires
		this->unlock();
		}).detach();
}

void App::unlock()
{
	this->networkLock = false;
}

App::~App()
{
	if (this->robots[0]) {
		delete this->robots[0];
	}
	
	if (this->robots[1]) {
		delete this->robots[1];
	}
	
	if (this->curSoc) {
		delete this->curSoc;
	}

	if (this->nexthop) {
		delete this->nexthop;
	}
}
