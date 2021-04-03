// SimpleClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <algorithm>
#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")


bool runClient(int port);
bool runServer(int port);

int main(int argc, char* argv[])
{
    auto printUsage = []() 
    {
        std::cout << "usage: simpleclient.exe MODE PORT" << std::endl;
        std::cout << "MODE:" << std::endl;
        std::cout << "-c    set-up program in client mode" << std::endl;
        std::cout << "-s    set-up program in server mode" << std::endl;
    };

    if (argc != 3)
    {
        printUsage();
        return 1;
    }

    int port = atoi(argv[2]);
    if (port < 1 || port > 65535)
    {
        std::cout << "invalid port. 0 <= PORT <= 65535 " << std::endl;
        printUsage();
        return 1;
    }

    WSADATA wsaData;
    int startupRes = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (startupRes != NO_ERROR)
    {
        MessageBoxA(NULL, "WSAStartup Error!", "ERROR", MB_OK);
        return 1;
    }

    int res;
    if (strcmp(argv[1], "-c") == 0)
    {
        
        std::cout << "starting client on port " << port  << "..." << std::endl;
        res = runClient(port);
    }
    else if (strcmp(argv[1], "-s") == 0)
    {
        std::cout << "starting server on port " << port << "..." << std::endl;
        res = runServer(port) ? 0 : 1; // Technically, runServer can be implicitly casted to int from bool
    }
    else
    {
        std::cout << "MODE required." << std::endl;
        printUsage();
        return 1;
    }

    WSACleanup();

    return res;
}

bool runClient(int port)
{
    SOCKET newSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    clientService.sin_port = htons(port);

    int res = connect(newSocket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (res == SOCKET_ERROR)
    {
        std::cout << "connect error. code: " << res << std::endl;
        return false;
    }

    std::cout << "connected to server on port " << port << std::endl;
    std::cout << "type 'exit()' to quit" << std::endl;

    bool error = false;
    std::string input;
    while (1)
    {
        std::cout << "send > ";
        std::getline(std::cin, input);
        
        if (input == "exit()")
        {
            std::cout << "exit() called. closing socket..." << std::endl;
            break;
        }

        int bytesSent = send(newSocket, input.c_str(), input.length()+1, 0);
        if (bytesSent == SOCKET_ERROR)
        {
            std::cout << "SOCKET_ERROR from send(...) | WSAGetLastError: " << WSAGetLastError() << std::endl;
            error = true;
            break;
        }
        else if (bytesSent != input.length()+1)
        {
            std::cout << "send size mismatch. bytesSent != strlen(buf)" << std::endl;
        }
    }

    closesocket(newSocket);
    return !error;
}

bool runServer(int port) 
{
    SOCKET listenSocket;
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        MessageBoxA(NULL, "socket failed with error", "", MB_OK);
        return false;
    }

    sockaddr_in serverService;
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverService.sin_port = htons(port);

    if (bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR) {
        std::cout << "Bind failed with error" << std::endl;
        closesocket(listenSocket);
        return false;
    }


    std::cout << "waiting for new connection..." << std::endl;
    if (listen(listenSocket, 1) == SOCKET_ERROR) {
        std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        return false;
    }

    SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET) {
        std::cout << "accept failed with error: " << acceptSocket << std::endl;
        closesocket(listenSocket);
        return false;
    }
    
    std::cout << "client connected on port " << port << std::endl;
    
    bool error = false;
    char buf[2048];
    int bytesRet;
    while ((bytesRet = recv(acceptSocket, buf, sizeof(buf), 0)) > 0)
    {
        if (bytesRet == SOCKET_ERROR)
        {
            std::cout << "SOCKET_ERROR from recv(...) | WSAGetLastError: " << WSAGetLastError() << std::endl;
            error = true;
            break;
        }

        std::cout << "Data received: " << buf << " | Length: " << bytesRet << std::endl;
    }

    std::cout << "connection lost. error: " << (error ? "true" : "false") << std::endl;

    closesocket(acceptSocket);
    closesocket(listenSocket);
    return !error;
}