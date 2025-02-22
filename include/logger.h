#include <WinSock2.h>

class Logger
{
private:
    SOCKET server;
    char* receiveBuffer;
    int receiveBufferSize;
    
public:
    Logger(int port);
    ~Logger();

    void Process();
    static void Log(const char *message);
};

