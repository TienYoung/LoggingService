#include "logger.h"

#include <iostream>
#include <format>
#include <chrono>

void Log(const char* message)
{
    std::cout << std::format("{:%FT%TZ}: Hello {}!", std::chrono::system_clock::now(), message);
}