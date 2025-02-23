#ifdef _WIN32
    #include <WinSock2.h>
    typedef SOCKET socket_t;
#else
    typedef int socket_t;
#endif

#include <vector>
#include <fstream>

class Logger
{
public:
    Logger(int port, const char *filename);
    ~Logger();

    void Process();
    void Log(const char *message);

private:
    socket_t m_server;
    int m_receiveBufferSize;
    std::vector<char> m_receiveBuffer;
    std::ofstream m_filestream;
};
