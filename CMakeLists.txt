cmake_minimum_required(VERSION 3.16)
project(LoggingSerivce LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(LoggingSerivce ${SOURCES})

if(WIN32)
    target_link_libraries(LoggingSerivce WS2_32)
endif()