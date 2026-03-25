#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <iostream>
#include "PktDef.h"


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
		TCP = 1,
		UDP = 2
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

	MySocket(SocketType, std::string ip, unsigned int port , ConnectionType, unsigned int bufferSize);
	~MySocket();
	void connectTCP();
	void connectUDP();
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

