/*
* FILE : main.cpp
* PROJECT : SENG2040 - ASSIGNMENT 3
* PROGRAMMER : Tian Yang, 8952896
*              Yu-Hsuan, Lee, 8904099
* FIRST VERSION : 2025-02-13
* DESCRIPTION :
*   This file contains the entry point for the logging service application. 
*   It loads configuration settings from a TOML file, parses command-line arguments, 
*   and initializes the logging service. The program runs indefinitely, processing log 
*   data based on the configured parameters such as port number, log file path, and rate limit.
*/

#include "logger.h"

#include <iostream>
#include <cstring>
#include <charconv>

enum OPTION {PORT, LOG_PATH, RATE_LIMIT, NUM} ;
const char* g_options[] = {"-p", "-l", "-r"};

int main(int argc, char* argv[])
{
    int port = 50000;
    std::string filename = "log.lnav";
    int rate_limit = 100;
    toml::value config;
    // Load config file.
    try
    {
        config= toml::parse("config.toml");

        port = config["Service"]["port_number"].as_integer();
        filename = config["Service"]["log_file"].as_string();
        rate_limit = config["Service"]["rate_limit"].as_integer();
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
        else if(strcmp(argv[i], g_options[RATE_LIMIT]) == 0)
        {
            auto [ptr, ec] = std::from_chars(argv[i+1], argv[i+1] + strlen(argv[i+1]), rate_limit); // Convert str to int
            if (ec != std::errc())
            {
                std::cerr << "This is not a valid rate limite!" << std::endl;
                return EXIT_FAILURE;
            }
            config["Service"]["rate_limit"].as_integer() = rate_limit;
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
    catch(const toml::syntax_error& e) // Config errors
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}