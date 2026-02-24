cmake_minimum_required(VERSION 4.1)
project(Reti_di_calcolatori)

set(CMAKE_CXX_STANDARD 20)

add_executable(Reti_di_calcolatori main.cpp
        Server.hpp
        Server.cpp
        Client.hpp
        Client.cpp
)
