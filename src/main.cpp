#include "logger.h"

#include <iostream>
#include <cstring>
#include <charconv>

enum OPTION {PORT, LOG_PATH, NUM} ;
const char* g_options[] = {"-p", "-l"};

int main(int argc, char* argv[])
{
    int port = 50000;
    std::string filename = "log.lnav";
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], g_options[PORT]) == 0)
        {
            auto [ptr, ec] = std::from_chars(argv[i+1], argv[i+1] + strlen(argv[i+1]), port); // Convert str to int
            if (ec != std::errc())
            {
                std::cerr << "This is not a valid port number!" << std::endl;
                return EXIT_FAILURE;
            }
        }
        else if(strcmp(argv[i], g_options[LOG_PATH]) == 0)
        {
            filename = argv[i+1];
        }
    }

    try
    {
        Logger log(port, filename.c_str());
        while (true)
        {
            log.Process();
        }
    }
    catch(const std::system_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}