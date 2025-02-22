#include "logger.h"

#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        Logger log(50000, "log.lnav");
        while (true)
        {
            log.Process();
        }
    }
    catch(const std::system_error& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}