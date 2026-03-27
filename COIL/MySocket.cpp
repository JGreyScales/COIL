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
	delete[] Buffer;
}

void MySocket::ConnectTCP()
{
	if (connectionType != TCP)
	{
		std::cerr << "Error: ConnectTCP can only be used with TCP sockets." << std::endl;
		return;
	}

	if (bTCPConnect)
	{
		std::cerr << "TCP connection is already established." << std::endl;
		return;
	}

	if (mySocket == CLIENT)
	{
		if (ConnectionSocket == INVALID_SOCKET)
		{
			std::cerr << "Error: Invalid client socket." << std::endl;
			return;
		}

		if (connect(ConnectionSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
		{
			std::cerr << "Error: TCP client failed to connect." << std::endl;
			return;
		}

		bTCPConnect = true;
	}
	else // SERVER
	{
		if (WelcomeSocket == INVALID_SOCKET)
		{
			std::cerr << "Error: Invalid welcome socket." << std::endl;
			return;
		}

		ConnectionSocket = accept(WelcomeSocket, NULL, NULL);
		if (ConnectionSocket == INVALID_SOCKET)
		{
			std::cerr << "Error: TCP server failed to accept connection." << std::endl;
			return;
		}

		bTCPConnect = true;
	}
}

void MySocket::DisconnectTCP()
{
	if (connectionType != TCP)
	{
		std::cerr << "Error: DisconnectTCP can only be used with TCP sockets." << std::endl;
		return;
	}

	if (ConnectionSocket != INVALID_SOCKET)
	{
		closesocket(ConnectionSocket);
		ConnectionSocket = INVALID_SOCKET;
	}

	bTCPConnect = false;

	// Recreate client TCP socket if needed so it can reconnect later
	if (mySocket == CLIENT)
	{
		ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectionSocket == INVALID_SOCKET)
		{
			std::cerr << "Error recreating TCP client socket." << std::endl;
		}
	}
}

void MySocket::SendData(char* data, int dataSize)
{
	if (data == nullptr || dataSize <= 0)
	{
		std::cerr << "Error: Invalid data to send." << std::endl;
		return;
	}

	if ((unsigned int)dataSize > maxSize)
	{
		std::cerr << "Error: Data size exceeds socket buffer size." << std::endl;
		return;
	}

	if (connectionType == TCP)
	{
		if (!bTCPConnect || ConnectionSocket == INVALID_SOCKET)
		{
			std::cerr << "Error: TCP socket is not connected." << std::endl;
			return;
		}

		int totalSent = 0;
		while (totalSent < dataSize)
		{
			int sentBytes = send(ConnectionSocket, data + totalSent, dataSize - totalSent, 0);
			if (sentBytes == SOCKET_ERROR || sentBytes == 0)
			{
				std::cerr << "Error sending TCP data." << std::endl;
				return;
			}
			totalSent += sentBytes;
		}
	}
	else // UDP
	{
		if (ConnectionSocket == INVALID_SOCKET)
		{
			std::cerr << "Error: Invalid UDP socket." << std::endl;
			return;
		}

		int sentBytes = sendto(
			ConnectionSocket,
			data,
			dataSize,
			0,
			(struct sockaddr*)&SvrAddr,
			sizeof(SvrAddr)
		);

		if (sentBytes == SOCKET_ERROR)
		{
			std::cerr << "Error sending UDP data." << std::endl;
			return;
		}
	}
}

int MySocket::GetData(char* data)
{
	if (data == nullptr)
	{
		std::cerr << "Error: Output buffer is null." << std::endl;
		return -1;
	}

	memset(data, 0, maxSize);

	if (connectionType == TCP)
	{
		if (!bTCPConnect || ConnectionSocket == INVALID_SOCKET)
		{
			std::cerr << "Error: TCP socket is not connected." << std::endl;
			return -1;
		}

		int receivedBytes = recv(ConnectionSocket, data, maxSize - 1, 0);
		if (receivedBytes == SOCKET_ERROR)
		{
			std::cerr << "Error receiving TCP data." << std::endl;
			return -1;
		}

		if (receivedBytes >= 0 && (unsigned int)receivedBytes < maxSize)
			data[receivedBytes] = '\0';

		return receivedBytes;
	}
	else // UDP
	{
		if (ConnectionSocket == INVALID_SOCKET)
		{
			std::cerr << "Error: Invalid UDP socket." << std::endl;
			return -1;
		}

		socklen_t addrLen = sizeof(SvrAddr);
		int receivedBytes = recvfrom(
			ConnectionSocket,
			data,
			maxSize - 1,
			0,
			(struct sockaddr*)&SvrAddr,
			&addrLen
		);

		if (receivedBytes == SOCKET_ERROR)
		{
			std::cerr << "Error receiving UDP data." << std::endl;
			return -1;
		}

		if (receivedBytes >= 0 && (unsigned int)receivedBytes < maxSize)
			data[receivedBytes] = '\0';

		return receivedBytes;
	}
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
