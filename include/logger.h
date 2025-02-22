#include <WinSock2.h>

#include <vector>
#include <fstream>

class Logger
{
private:
    SOCKET m_server;
    int m_receiveBufferSize;
    std::vector<char> m_receiveBuffer;
    std::ofstream m_filestream;
    
public:
    Logger(int port, const char* filename);
    ~Logger();

    void Process();
    void Log(const char *message);
};

