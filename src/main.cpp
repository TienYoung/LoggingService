#include "logger.h"

#include <iostream>
#include <cstring>
#include <charconv>

enum OPTION {PORT, LOG_PATH, RATE_LIMITE, NUM} ;
const char* g_options[] = {"-p", "-l", "-r"};

int main(int argc, char* argv[])
{
    int port = 50000;
    std::string filename = "log.lnav";
    int rate_limite = 100;
    toml::value config;
    // Load config file.
    try
    {
        config= toml::parse("config.toml");

        port = config["Service"]["port_number"].as_integer();
        filename = config["Service"]["log_file"].as_string();
        rate_limite = config["Service"]["rate_limite"].as_integer();
    }
    catch(const toml::file_io_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(const toml::type_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(const toml::syntax_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Parse arguments.
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
            config["Service"]["port_number"].as_integer() = port;
        }
        else if(strcmp(argv[i], g_options[LOG_PATH]) == 0)
        {
            filename = argv[i+1];
            config["Service"]["log_file"].as_string() = filename;
        }
        else if(strcmp(argv[i], g_options[RATE_LIMITE]) == 0)
        {
            auto [ptr, ec] = std::from_chars(argv[i+1], argv[i+1] + strlen(argv[i+1]), rate_limite); // Convert str to int
            if (ec != std::errc())
            {
                std::cerr << "This is not a valid rate limite!" << std::endl;
                return EXIT_FAILURE;
            }
            config["Service"]["rate_limite"].as_integer() = rate_limite;
        }
    }

    // Print the service config.
    std::cout << "Logging Service started." << std::endl;
    std::cout << config << std::endl;

    // Launch loggging service.
    try
    {
        Logger log(config["Service"], config["Format"]);
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
    catch(const toml::syntax_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}