#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <cstring> 

#include "Utils.h"

// --- Cross-Platform Compatibility Layer ---
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

typedef int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR    (-1)
#define closesocket(s)  close(s)
#endif

class MySocket {
public:
    enum SocketType { CLIENT, SERVER };
    enum ConnectionType { TCP = 1, UDP = 2 , UNDEFINED};
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
    MySocket(SocketType, std::string ip, unsigned int port, ConnectionType, unsigned int bufferSize);
    ~MySocket();

    void ConnectTCP();
    void DisconnectTCP();
    void SendData(char*, int);
    int GetData(char*);
    bool HasData(int);
    void BindLocalIP(std::string);

    std::string GetIPAddr();
    void SetIPAddr(std::string ip);
    void SetPort(int p);
    int GetPort();
    SocketType GetType();
    void SetType(SocketType t);
};