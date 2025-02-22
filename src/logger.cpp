#include "logger.h"

#include <iostream>
#include <format>
#include <chrono>
#include <cassert>
#include <exception>

#define CHECK_WSA(call)                                                                               \
    do                                                                                                \
    {                                                                                                 \
        int errorCode = call;                                                                         \
        if (errorCode != 0)                                                                           \
        {                                                                                             \
            throw std::system_error(errorCode, std::system_category(), "Failed to init Winsock DLL"); \
        }                                                                                             \
    } while (false)

#define CHECK_SOCKET(call)                                                                        \
    do                                                                                            \
    {                                                                                             \
        if (call == INVALID_SOCKET)                                                               \
        {                                                                                         \
            throw std::system_error(WSAGetLastError(), std::system_category(), "Invalid socket"); \
        }                                                                                         \
    } while (false)

#define CHECK_SOCK_ERROR(call)                                                                  \
    do                                                                                          \
    {                                                                                           \
        if (call == SOCKET_ERROR)                                                               \
        {                                                                                       \
            throw std::system_error(WSAGetLastError(), std::system_category(), "Socket error"); \
        }                                                                                       \
    } while (false)

Logger::Logger(int port, const char *filename)
    : m_server(0), m_receiveBufferSize(0), m_receiveBuffer(0), m_filestream(filename, std::ios::app)
{
    WSAData wsadata = {0};
    CHECK_WSA(WSAStartup(MAKEWORD(2, 2), &wsadata)); // initialize winsock dll

    m_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create a ipv4 tcp socket
    CHECK_SOCKET(m_server);

    sockaddr_in localAddrss = {0};
    localAddrss.sin_family = AF_INET;                                                                  // ipv4
    localAddrss.sin_addr.s_addr = inet_addr("127.0.0.1");                                              // local host
    localAddrss.sin_port = htons(port);                                                                // user specified port number
    CHECK_SOCK_ERROR(bind(m_server, reinterpret_cast<sockaddr *>(&localAddrss), sizeof(localAddrss))); // bind socket to the local host
    CHECK_SOCK_ERROR(listen(m_server, SOMAXCONN));                                                     // listen to this socket

    int optlen = sizeof(int);
    CHECK_SOCK_ERROR(getsockopt(m_server, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&m_receiveBufferSize), &optlen)); // get the socket buffer size
    m_receiveBuffer = std::vector<char>(m_receiveBufferSize, 0);                                                            // new a receive buffer
}

Logger::~Logger()
{
    closesocket(m_server);
    WSACleanup();
    m_filestream.close();
}

void Logger::Process()
{
    sockaddr_in clientAddress = {0};
    int clientAddrLen = sizeof(sockaddr_in);
    SOCKET client = accept(m_server, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddrLen); // waiting for a client connection
    CHECK_SOCKET(client);

    int receivedSize = recv(client, m_receiveBuffer.data(), m_receiveBufferSize, 0);
    CHECK_SOCK_ERROR(receivedSize);
    if (receivedSize == 0)
    {
        std::cout << "Connection closed" << std::endl;
    }
    else
    {

        Log(m_receiveBuffer.data());
    }
}

void Logger::Log(const char *message)
{
    m_filestream << std::format("{0:%F}T{0:%TZ}: {1}", std::chrono::system_clock::now(), message) << std::endl;
}