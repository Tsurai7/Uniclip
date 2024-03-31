#include "Clipboard.h"
#include "../Network/Network.h"
#include "../Logging/Logging.h"

#include <unistd.h>


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


void runSetClipCommand(const char* text) {
    #ifdef __APPLE__
        return setClipCommand("pbcopy", text);
    #elif __linux__
        return setClipCommand("xclip -selection clipboard", text);
    #elif _WIN32
        return setClipCommand(powershell.exe -command -clip", text);
    #else
        exit(EXIT_FAILURE);
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

    if (status < 0) {
        return "Error closing pipe!";
    }

    return result;
}


void setClipCommand(const char* command, const char* text) {

    FILE *pipe = popen(command, "w");

    if (!pipe) {
        printf("Ошибка при открытии потока для команды pbcopy\n");
        exit(EXIT_FAILURE);
    }

    fprintf(pipe, "%s", text);

    int status = pclose(pipe);

    if (status < 0) {
        printf("Ошибка при закрытии потока для команды pbcopy\n");
        exit(EXIT_FAILURE);
    }

    logger("CLIPBOARD SET LOCALLY", text);
}

void* manageClip(void* arg) {
    std::string startClip = "";

    while (1) {
        std::string localClip = runGetClipCommand();

        if (localClip != startClip) {

            startClip = localClip;

            logger("CLIPBOARD CHANGED LOCALLY", localClip.c_str());

            sendBroadcast(localClip.c_str());

        }

        sleep(1); // !!! sending extra space without sleep (maybe some troubles with threads)
    }
}
