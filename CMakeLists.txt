cmake_minimum_required(VERSION 3.27)
project(Uniclip)

set(CMAKE_CXX_STANDARD 17)


add_executable(Uniclip uniclip.cpp
        Utils/Network/Network.cpp
        Utils/Network/Network.h
        Utils/Clipboard/Clipboard.cpp
        Utils/Clipboard/Clipboard.h
        Utils/Crypto/Crypto.cpp
        Utils/Crypto/Crypto.h
        Utils/Logging/Logging.cpp
        Utils/Logging/Logging.h
        Utils/Notifications/Notify.cpp
        Utils/Notifications/Notify.h
)