cmake_minimum_required(VERSION 3.16)
project(LoggingService LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(LoggingService ${SOURCES})

if(WIN32)
    target_link_libraries(LoggingService WS2_32)
endif()

add_custom_command(TARGET LoggingService POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/config.toml
    $<TARGET_FILE_DIR:LoggingService>/config.toml
)