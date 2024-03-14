#include "Clipboard.h"

#include <iostream>


std::string runGetClipCommand() {
    #ifdef __APPLE__
        return getClipCommand("pbpaste");
    #elif __linux__
        return getClipCommand("xclip -o");
    #elif _WIN32
        return getClipCommand("powershell.exe -command Get-clipboard");
    #else
        return "Error: unknown operating system.\n";
    #endif
}

void runSetClipCommand(char* text) {
    #ifdef __APPLE__
        return setClipCommand("pbcopy", text);
    #elif __linux__
        return setClipCommand("xclip -selection clipboard", text);
    #elif _WIN32
        return setClipCommand(powershell.exe -command -clip", text);
    #else
        return;
    #endif
}

std::string getClipCommand(const char* command) {
    char buffer[1024];
    std::string result;

    FILE* pipe = popen(command, "r");

    if (!pipe) {
        return "Error opening pipe!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status == -1) {
        return "Error closing pipe!" ;
    }

    return result;
}

void setClipCommand(const char* command, std::string text) {
    FILE *pipe = popen(command, "w");

    if (!pipe) {
        std::cerr << "Ошибка при открытии потока для команды pbcopy" << std::endl;
        return;
    }

    fprintf(pipe, "%s", text.c_str());

    int status = pclose(pipe);

    if (status == -1) {
        std::cerr << "Ошибка при закрытии потока для команды pbcopy" << std::endl;
        return;
    }

    return;
}