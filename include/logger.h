#ifdef _WIN32
    typedef SOCKET socket_t;
#else
    typedef int socket_t;
#endif

#include <vector>
#include <fstream>

class Logger
{
private:
    socket_t m_server;
    int m_receiveBufferSize;
    std::vector<char> m_receiveBuffer;
    std::ofstream m_filestream;
    
public:
    Logger(int port, const char* filename);
    ~Logger();

    void Process();
    void Log(const char *message);
};

