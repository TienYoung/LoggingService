#include "logger.h"

#include <iostream>
#include <format>
#include <chrono>
#include <cassert>

Logger::Logger(int port) : server(0), receiveBuffer(nullptr), receiveBufferSize(0)
{
    WSAData wsadata = {0};
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in localAddrss = {0};
    localAddrss.sin_family = AF_INET;
    localAddrss.sin_addr.s_addr = inet_addr("127.0.0.1");
    localAddrss.sin_port = htons(port);

    bind(server, reinterpret_cast<sockaddr *>(&localAddrss), sizeof(localAddrss));

    listen(server, SOMAXCONN);

    int optlen = sizeof(int);
    if (getsockopt(server, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&receiveBufferSize), &optlen) != SOCKET_ERROR)
    {
        receiveBuffer = new char[receiveBufferSize];
        memset(receiveBuffer, '\0', receiveBufferSize);
    }
    else
    {
        int error = WSAGetLastError();
        std::cout << error << std::endl;
    }
}

void Logger::Process()
{
    sockaddr_in clientAddress = {0};
    int clientAddrLen = sizeof(sockaddr_in);
    SOCKET client = accept(server, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddrLen);
    if(client == INVALID_SOCKET)
    {
        int error = WSAGetLastError();
        std::cout << error << std::endl;
    }

    int i = 0;
    do
    {
        i = recv(client, receiveBuffer, receiveBufferSize, 0);
        if (i > 0)
        {
            Log(receiveBuffer);
        }
        else if (i == 0)
        {
            std::cout << "connection closed" << std::endl;
        }
        else
        {
            int error = WSAGetLastError();
            std::cout << error << std::endl;
        }
    } while (i > 0);
}

Logger::~Logger()
{
    delete[] receiveBuffer;
}

void Logger::Log(const char *message)
{
    std::cout << std::format("{:%FT%TZ}: Hello {}!\n", std::chrono::system_clock::now(), message);
}