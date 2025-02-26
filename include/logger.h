#ifdef _WIN32
    #include <WinSock2.h>
    typedef SOCKET socket_t;
#else
    typedef int socket_t;
#endif

#include <vector>
#include <fstream>
#include <unordered_map>
#include <chrono>

#include "toml.hpp"

class Logger
{
public:
    Logger(const toml::value& service, const toml::value& format);
    ~Logger();

    void Process();
    void Log(int priority, const char* timestamp, const char* hostname, const char* application, 
        int pid, const char* msgid, const char *message);

private:
    socket_t m_server;
    int m_receiveBufferSize;
    std::vector<char> m_receiveBuffer;
    std::ofstream m_filestream;
    std::unordered_map<std::string, std::chrono::time_point<
        std::chrono::system_clock, std::chrono::milliseconds>> m_lastConnectionTime;
    std::chrono::milliseconds m_minimumTimeInterval;

    bool m_supportPriority;
    bool m_supportTimestamp;
    bool m_supportHostname;
    bool m_supportApplication;
    bool m_supportPid;
    bool m_supportMsgid;
    bool m_supportMessage;
};
