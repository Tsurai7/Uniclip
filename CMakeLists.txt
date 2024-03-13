cmake_minimum_required(VERSION 3.27)
project(Uniclip)

set(CMAKE_CXX_STANDARD 17)

add_executable(Uniclip uniclip.cpp
        Utils/Server/Server.cpp
        Utils/Server/Server.h
        Utils/Clipboard/Clipboard.cpp
        Utils/Clipboard/Clipboard.h
        Utils/Client/Client.cpp
        Utils/Client/Client.h)