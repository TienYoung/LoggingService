#include "logger.h"

#include <iostream>
#include <format>
#include <cassert>
#include <exception>

#ifdef _WIN32
    #include <WS2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
#endif

#include "json.hpp"
using json = nlohmann::json;

#ifdef _WIN32
    #define CHECK_WSA(call) do {                                                                     \
        int errorCode = call;                                                                        \
        if (errorCode != 0)                                                                          \
        {                                                                                            \
            throw std::system_error(errorCode, std::system_category(), "Failed to init Winsock DLL");\
        }                                                                                            \
    } while (false)

    #define CHECK_SOCKET(call) do {                                                              \
        if (call == INVALID_SOCKET)                                                              \
        {                                                                                        \
            throw std::system_error(WSAGetLastError(), std::system_category(), "Invalid socket");\
        }                                                                                        \
    } while (false)

    #define CHECK_SOCK_ERROR(call) do {                                                        \
        if (call == SOCKET_ERROR)                                                              \
        {                                                                                      \
            throw std::system_error(WSAGetLastError(), std::system_category(), "Socket error");\
        }                                                                                      \
    } while (false)
#else
    #define CHECK_SOCKET(call) do {                                                  \
        if (call == -1)                                                              \
        {                                                                            \
            throw std::system_error(errno, std::system_category(), "Invalid socket");\
        }                                                                            \
    } while (false)

    #define CHECK_SOCK_ERROR(call) do {                                            \
        if (call == -1)                                                            \
        {                                                                          \
            throw std::system_error(errno, std::system_category(), "Socket error");\
        }                                                                          \
    } while (false)
#endif


Logger::Logger(const toml::value& service, const toml::value& format)
    : m_server(0), m_receiveBufferSize(0), m_receiveBuffer(0), m_filestream(service.at("log_file").as_string(), std::ios::app)
{
#ifdef _WIN32
    WSAData wsadata = {0};
    CHECK_WSA(WSAStartup(MAKEWORD(2, 2), &wsadata)); // initialize winsock dll
#endif
    m_server = socket(AF_INET, SOCK_STREAM, 0); // create a ipv4 tcp socket
    CHECK_SOCKET(m_server);

    sockaddr_in localAddrss = {0};
    localAddrss.sin_family = AF_INET;                                                                  // ipv4
    localAddrss.sin_addr.s_addr = inet_addr("0.0.0.0");                                                // local host
    localAddrss.sin_port = htons(service.at("port_number").as_integer());                              // user specified port number
    CHECK_SOCK_ERROR(bind(m_server, reinterpret_cast<sockaddr *>(&localAddrss), sizeof(localAddrss))); // bind socket to the local host
    CHECK_SOCK_ERROR(listen(m_server, SOMAXCONN));                                                     // listen to this socket

    socklen_t optlen = sizeof(int);
    CHECK_SOCK_ERROR(getsockopt(m_server, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&m_receiveBufferSize), &optlen)); // get the socket buffer size
    m_receiveBuffer = std::vector<char>(m_receiveBufferSize, 0);                                                            // new a receive buffer

    m_minimumTimeInterval = std::chrono::milliseconds(1000 / service.at("rate_limit").as_integer());

    m_supportPriority = format.at("priority").as_boolean();
    m_supportTimestamp = format.at("timestamp").as_boolean();
    m_supportHostname = format.at("hostname").as_boolean();
    m_supportApplication = format.at("application").as_boolean();
    m_supportPid = format.at("pid").as_boolean();
    m_supportMsgid = format.at("msgid").as_boolean();
    m_supportMessage = format.at("message").as_boolean();
}

Logger::~Logger()
{
#ifdef _WIN32
    closesocket(m_server);
    WSACleanup();
#else
    close(m_server);
#endif
    m_filestream.close();
}

void Logger::Process()
{
    sockaddr_in clientAddress = {0};
    socklen_t clientAddrLen = sizeof(sockaddr_in);
    socket_t client = accept(m_server, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddrLen); // waiting for a client connection
    CHECK_SOCKET(client);

    int receivedSize = recv(client, m_receiveBuffer.data(), m_receiveBufferSize, 0);
    CHECK_SOCK_ERROR(receivedSize);
    if (receivedSize == 0)
    {
        std::cout << "Connection closed" << std::endl;
    }
    else
    {
        std::cout << "Recieved: \n" << m_receiveBuffer.data() << std::endl;

        int priority = 0;
        std::string timestamp = std::format("{0:%F}T{0:%TZ}", std::chrono::system_clock::now());
        std::string hostname = "unknown";
        std::string application = "unknown";
        int pid = -1;
        std::string msgid = "-";
        std::string message = "";

        try
        {
            const json data = json::parse(m_receiveBuffer.data());
            if(m_supportPriority == true)
            {
                priority = data["priority"];
            }
            if(m_supportTimestamp == true)
            {
                timestamp = data["timestamp"];
            }
            if(m_supportHostname == true)
            {
                hostname = data["hostname"];
            }
            if(m_supportApplication == true)
            {
                application = data["application"];
            }
            if(m_supportPid == true)
            {
                pid = data["pid"];
            }
            if(m_supportMsgid == true)
            {
                msgid = data["msgid"];
            }
            if(m_supportMessage == true)
            {
                message = data["message"];
            }

            auto currentTimePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            auto it = m_lastConnectionTime.find(hostname + application);
            if(it == m_lastConnectionTime.end()) // If is the first time connection
            {
               m_lastConnectionTime.emplace(hostname + application, currentTimePoint);
            }
            else
            {
                auto deltaTime = currentTimePoint - it->second;
                it->second = currentTimePoint;
                if(deltaTime < m_minimumTimeInterval)
                {
                    // Refuse.
                    std::cout << std::format("Refuse {} {}, which is too noisy.", hostname, application) << std::endl;
                #ifdef _WIN32
                    closesocket(client);
                #else
                    close(client);
                #endif
                    return;
                }
            }

            Log(priority, timestamp.c_str(), hostname.c_str(), application.c_str(), pid, msgid.c_str(), message.c_str());
        }
        catch(const json::type_error& e)
        {
            std::cerr << "Not a valid log type." << std::endl;
            std::cerr << e.what() << std::endl;
        }
        catch(const json::parse_error& e)
        {
            std::cerr << "Not a valid log format." << std::endl;
            std::cerr << e.what() << std::endl;
        }

    #ifdef _WIN32
        closesocket(client);
    #else
        close(client);
    #endif
        return;
    }
}

void Logger::Log(int priority, const char* timestamp, const char* hostname, const char* application, 
    int pid, const char* msgid, const char *message)
{
    m_filestream << std::format("<{0}>1 {1} {2} {3} {4} {5} {6}", priority, timestamp, hostname, application, pid, msgid, message) << std::endl;
}