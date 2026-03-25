#include "MySocket.h"

MySocket::MySocket(SocketType socketType, std::string ipAddr, unsigned int portNum, ConnectionType connType, unsigned int size)
{
	// Validate buffer size
	if (size == 0)
		maxSize = DEFAULT_SIZE;
	else
		maxSize = size;

	Buffer = new char[maxSize];
	memset(Buffer, 0, maxSize);

	mySocket = socketType;
	IPAddr = ipAddr;
	port = portNum;
	connectionType = connType;
	bTCPConnect = false;
	WelcomeSocket = INVALID_SOCKET;
	ConnectionSocket = INVALID_SOCKET;

	// Initialize Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed." << std::endl;
		return;
	}

	// Configure address structure
	memset(&SvrAddr, 0, sizeof(SvrAddr));
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(port);
	inet_pton(AF_INET, IPAddr.c_str(), &SvrAddr.sin_addr);

	if (mySocket == SERVER)
	{
		if (connectionType == TCP)
		{
			// Create TCP welcome socket for server
			WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (WelcomeSocket == INVALID_SOCKET)
			{
				std::cerr << "Error creating TCP welcome socket." << std::endl;
				return;
			}
			if (bind(WelcomeSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
			{
				std::cerr << "Error binding TCP socket." << std::endl;
				closesocket(WelcomeSocket);
				return;
			}
			if (listen(WelcomeSocket, 1) == SOCKET_ERROR)
			{
				std::cerr << "Error listening on TCP socket." << std::endl;
				closesocket(WelcomeSocket);
				return;
			}
			// Accept a connection
			ConnectionSocket = accept(WelcomeSocket, NULL, NULL);
			if (ConnectionSocket == INVALID_SOCKET)
			{
				std::cerr << "Error accepting TCP connection." << std::endl;
				closesocket(WelcomeSocket);
				return;
			}
			bTCPConnect = true;
		}
		else // UDP
		{
			ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (ConnectionSocket == INVALID_SOCKET)
			{
				std::cerr << "Error creating UDP socket." << std::endl;
				return;
			}
			if (bind(ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
			{
				std::cerr << "Error binding UDP socket." << std::endl;
				closesocket(ConnectionSocket);
				return;
			}
		}
	}
	else // CLIENT
	{
		if (connectionType == TCP)
		{
			ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (ConnectionSocket == INVALID_SOCKET)
			{
				std::cerr << "Error creating TCP client socket." << std::endl;
				return;
			}
		}
		else // UDP
		{
			ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (ConnectionSocket == INVALID_SOCKET)
			{
				std::cerr << "Error creating UDP client socket." << std::endl;
				return;
			}
		}
	}
}

MySocket::~MySocket()
{
	if (ConnectionSocket != INVALID_SOCKET)
		closesocket(ConnectionSocket);
	if (WelcomeSocket != INVALID_SOCKET)
		closesocket(WelcomeSocket);
	WSACleanup();
	delete[] Buffer;
}

std::string MySocket::GetIPAddr()
{
	return IPAddr;
}

void MySocket::SetIPAddr(std::string ipAddr)
{
	if (bTCPConnect)
	{
		std::cerr << "Error: Cannot change IP address while a connection is established." << std::endl;
		return;
	}
	IPAddr = ipAddr;
}

void MySocket::SetPort(int portNum)
{
	if (bTCPConnect)
	{
		std::cerr << "Error: Cannot change port while a connection is established." << std::endl;
		return;
	}
	port = portNum;
}

int MySocket::GetPort()
{
	return port;
}

MySocket::SocketType MySocket::GetType()
{
	return mySocket;
}

void MySocket::SetType(SocketType socketType)
{
	mySocket = socketType;
}
