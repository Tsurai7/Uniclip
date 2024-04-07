#include "Clipboard.h"
#include "../Network/Network.h"
#include "../Logging/Logging.h"

#include <unistd.h>
#include <cstring>


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
    std::string startClip = runGetClipCommand();

    while (1) {
        std::string localClip = runGetClipCommand();

        if (localClip != startClip) {

            startClip = localClip;

            logger("CLIPBOARD CHANGED LOCALLY", localClip.c_str());

            send_to_all_tcp(localClip.c_str());
        }

        sleep(1); // !!! sending extra space without sleep (maybe some troubles with threads)
    }
}

const char* findFile(const char* fileName) {
    std::string command = "find /home -name \'" + std::string(fileName) + "\'";

    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        printf("Ошибка при открытии потока для команды find\n");
        exit(EXIT_FAILURE);
    }


    // Считываем результаты поиска
    char result[1024];
    fgets(result, sizeof(result), pipe);

    int status = pclose(pipe);
    if (status < 0) {
        printf("Ошибка при закрытии потока для команды find\n");
        exit(EXIT_FAILURE);
    }

    // Обрезаем символ новой строки, который fgets мог добавить
    char *newline = strchr(result, '\n');
    if (newline) *newline = '\0';

    // Если файл найден, возвращаем его путь
    if (result[0] != '\0') {
        return strdup(result); // Выделяем память под копию строки
    } else {
        return NULL; // Если файл не найден, возвращаем NULL
    }
}
