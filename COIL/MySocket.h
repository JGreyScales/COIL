#pragma once


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <string>
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
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

	static const unsigned int DEFAULT_SIZE = 255;

private:
	char* Buffer;
	SOCKET WelcomeSocket;
	SOCKET ConnectionSocket;
	sockaddr_in SvrAddr;
	SocketType mySocket;
	std::string IPAddr;
	int port;
	ConnectionType connectionType;
	bool bTCPConnect;
	unsigned int maxSize;
public:
	MySocket(SocketType, std::string ip, unsigned int port , ConnectionType, unsigned int bufferSize);
	~MySocket();
	void ConnectTCP();
	void DisconnectTCP();
	void SendData(char*, int);
	int GetData(char*);
	std::string GetIPAddr();
	void SetIPAddr(std::string);
	void SetPort(int);
	int GetPort();
	SocketType GetType();
	void SetType(SocketType);
};

