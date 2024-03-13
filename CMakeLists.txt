cmake_minimum_required(VERSION 3.27)
project(Uniclip)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)

add_executable(Uniclip uniclip.cpp
        Utils/Network/Network.cpp
        Utils/Network/Network.h
        Utils/Clipboard/Clipboard.cpp
        Utils/Clipboard/Clipboard.h
        Utils/Data/Data.cpp
        Utils/Data/Data.h
        Utils/Crypto/Crypto.cpp
        Utils/Crypto/Crypto.h)

target_link_libraries(Uniclip PRIVATE ZLIB::ZLIB)