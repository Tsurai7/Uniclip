#ifndef UNICLIP_CRYPTO_H
#define UNICLIP_CRYPTO_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>



enum MessageType {
    TEXT_MESSAGE,
    FILE_MESSAGE,
    DIRECTORY_MESSAGE
};

char* find(const char *dir_path, const char* entry_name, MessageType *info);


#endif