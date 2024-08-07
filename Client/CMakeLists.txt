cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(Client/include)

add_executable(Client
    Client/src/Main.cpp
    Client/src/Client.cpp
)
