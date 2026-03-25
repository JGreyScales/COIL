#pragma once

#include <cstdint>
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>
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
	MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
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

