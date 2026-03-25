#pragma once

#include <cstdint>
#include <socketapi.h>

#include <winsock2.h>
#include <iostream>

// Link the Winsock library
#pragma comment(lib, "ws2_32.lib")


class MySocket
{
public:
	enum SocketType
	{
		CLIENT,
		SERVER
	};

	enum ConnectionType
	{
		TCP,
		UDP
	};

	static const uint8_t DEFAULT_SIZE = 255;
	char* Buffer;
	SOCKET WelcomeSocket;
	SOCKET ConnectionSocket;
	sockaddr_in SvrAddr;
	std::string IPAddr;
	int port;
	ConnectionType connectionType;
	bool bTCPConnect;
	int maxSize;

	//

	MySocket(SocketType, unsigned int, ConnectionType);
	~MySocket();
	void connectTCP();
	void DisconnectionTCP();
	void SendData(char*, int);
	int getData(char*);
	std::string getIPAddr();
	void setIPAddr(std::string);
	void setPort();
	int getPort();
	SocketType GetSocketType();
	void SetSocketType(SocketType);
};

