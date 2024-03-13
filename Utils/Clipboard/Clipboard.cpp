#include "Clipboard.h"


string runGetClipCommand() {
    #ifdef __APPLE__
        return getClipDarwin();
    #elif __linux__
        return getClipLinux();
        #elif _WIN32
            return getClipWin();
        #else
            return "Unknown operating system.\n";
    #endif
}

string getClipLinux() {
    char buffer[1024];
    string result;

    FILE* pipe = popen("xclip -o", "r");

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

string getClipDarwin() {
    char buffer[1024];
    string result;

    FILE *pipe = popen("pbpaste", "r");

    if (!pipe) {
        return "Error opening pipe!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status == -1) {
        return "Error closing pipe!";
    }

    return result;
}

string getClipWin() {
    char buffer[1024];
    string result;

    FILE* pipe = popen("powershell.exe -command Get-clipboard", "r");

    if (!pipe) {
        return "Error opening pipe!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status == -1) {
        return "Error closing pipe!";
    }

    return result;
}
