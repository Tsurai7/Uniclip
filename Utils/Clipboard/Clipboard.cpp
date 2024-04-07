#include "Clipboard.h"
#include "../Network/Network.h"
#include "../Logging/Logging.h"

#include <unistd.h>
#include <cstring>


std::string run_get_clip_command()
{
    #ifdef __APPLE__
        return get_clip_command("pbpaste");
    #elif __linux__
        return get_clip_command("xclip -o");;
    #else
        return "Error: unknown operating system.\n";
    #endif
}


void run_set_clip_command(const char* text)
{
    #ifdef __APPLE__
        return set_clip_command("pbcopy", text);
    #elif __linux__
        return set_clip_command("xclip -selection clipboard", text);
    #else
        return "Error: unknown operating system.\n";
    #endif
}


std::string get_clip_command(const char* command)
{
    char buffer[1024];

    std::string result;

    FILE* pipe = popen(command, "r");

    if (!pipe)
    {
        return "Error opening pipe!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status < 0)
    {
        return "Error closing pipe!";
    }

    return result;
}


void set_clip_command(const char* command, const char* text)
{
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


void* manage_clip(void* arg)
{
    std::string startClip = run_get_clip_command();

    while (true)
    {
        std::string localClip = run_get_clip_command();

        if (localClip != startClip)
        {
            startClip = localClip;

            logger("CLIPBOARD CHANGED LOCALLY", localClip.c_str());

            send_to_all_tcp(localClip.c_str());
        }

        sleep(1); // !!! sending extra space without sleep (maybe some troubles with threads)
    }
}