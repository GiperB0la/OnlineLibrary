cmake_minimum_required(VERSION 3.10)
project(Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
    Server/include
)

add_executable(Server
    Server/src/Main.cpp
    Server/src/Library.cpp
    Server/src/Server.cpp
)