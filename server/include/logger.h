/*
* FILE : logger.h
* PROJECT : SENG2040 - ASSIGNMENT 3
* PROGRAMMER : Tian Yang, 8952896
*              Yu-Hsuan, Lee, 8904099
* FIRST VERSION : 2025-02-13
* DESCRIPTION :
*   This file defined the `Logger` class, which is responsible for receiving 
*   log messages over a TCP socket, processing them according to a specified 
*   format, and writing them to a log file. It supports configurable logging 
*   fields, rate limiting, and structured logging using JSON.
*/
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

/*
 * Class : Logger
 * Description :
 *   This class provides a logging service that listens for log messages over 
 *   a TCP socket, processes them based on a configured format, and writes 
 *   them to a log file. It supports structured logging, rate limiting, 
 *   and configurable log fields.
 */
class Logger
{
public:
    /*
    * Function : Logger
    * Description :
    *   Constructor for the Logger class. Initializes the logging service 
    *   by setting up the network socket and loading configuration options.
    * Parameters :
    *   const toml::value& service - Configuration parameters for the logging service (e.g., port, log file path, rate limit).
    *   const toml::value& format - Configuration for log format (e.g., enabled fields such as priority, timestamp, hostname, etc.).
    * Return :
    *   None
    */
    Logger(const toml::value& service, const toml::value& format);

    /*
    * Function : ~Logger
    * Description :
    *   Destructor for the Logger class. Cleans up resources, including 
    *   closing the network socket and log file stream.
    * Parameters :
    *   None
    * Return :
    *   None
    */
    ~Logger();

    /*
    * Function : Process
    * Description :
    *   Listens for incoming log messages over the TCP socket, processes 
    *   the received data, applies rate limiting, and filters based on 
    *   configured log fields before passing it to the logging system.
    * Parameters :
    *   None
    * Return :
    *   void
    */
    void Process();

    /*
    * Function : Log
    * Description :
    *   Writes a formatted log entry to the log file, following the specified 
    *   structure and format settings.
    * Parameters :
    *   int priority - Log priority level.
    *   const char* timestamp - Timestamp of the log entry.
    *   const char* hostname - Hostname of the system generating the log.
    *   const char* application - Application name generating the log.
    *   int pid - Process ID of the logging application.
    *   const char* msgid - Message ID for categorizing log entries.
    *   const char* message - Actual log message content.
    * Return :
    *   void
    */
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
