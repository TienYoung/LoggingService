#include <WinSock2.h>

#include <vector>

class Logger
{
private:
    SOCKET server;
    int receiveBufferSize;
    std::vector<char> receiveBuffer;
    
public:
    Logger(int port);
    ~Logger();

    void Process();
    static void Log(const char *message);
};

