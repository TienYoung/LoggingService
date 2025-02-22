#include "logger.h"

int main(int argc, char* argv[])
{
    Logger log(50000);
    while (true)
    {
        log.Process();
    }

    return 0;
}