#include "Clipboard.h"
#include "../Network/Network.h"
#include <sys/stat.h>

#define BUFFER_SIZE 1024

data_info run_get_clip_command()
{
    #ifdef __APPLE__
        std::string clipboard = get_clip_command("osascript -e 'get POSIX path of (the clipboard as «class furl»)'").FilePath;
        if (std::count_if( clipboard.begin(),clipboard.end(), [=](char c){ return c == '/'; }) > 1) {
            return get_clip_command("osascript -e 'get POSIX path of (the clipboard as «class furl»)'");
        }
        else {
            return get_clip_command("pbpaste");
        }
    #elif __linux__
        return get_clip_command("xclip -o");;
    #else
        return "ERROR: unknown operating system.\n";
    #endif
}

void run_set_clip_command(const char* text)
{
    #ifdef __APPLE__
        return set_clip_command("pbcopy", text);
    #elif __linux__
        return set_clip_command("xclip -selection clipboard", text);
    #else
        return "ERROR: unknown operating system.\n";
    #endif
}

data_info get_clip_command(const char* command)
{
    data_info info;
    char buffer[BUFFER_SIZE];
    std::string clipboard;

    FILE* pipe = popen(command, "r");

    if (!pipe) {
        perror("ERROR: with opening the pipe to get clip\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        clipboard += buffer;

    int status = pclose(pipe);

    if (status < 0) {
        perror("ERROR: with closing the pipe to get clip\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(command, "osascript -e 'get POSIX path of (the clipboard as «class furl»)'") == 0) {

        if(!clipboard.empty())
            clipboard.erase(clipboard.length() - 1);

        struct stat path_stat;
        stat(clipboard.c_str(), &path_stat);

        if (S_ISREG(path_stat.st_mode)) {
            info.Type = File;
            info.FilePath = clipboard;
            info.FileName = clipboard.substr(clipboard.find_last_of('/') + 1);
        }

        else {
            info.Type = Text;
            info.Data = clipboard.substr(clipboard.find_last_of('/') + 1);
            info.FileName = "";
            info.FilePath = "";
        }
    }
    else {
        info.Type = Text;
        info.Data = clipboard;
    }
    return info;
}

void set_clip_command(const char* command, const char* text)
{
    FILE *pipe = popen(command, "w");

    if (!pipe) {
        perror("ERROR: with opening the pipe for pbcopy\n");
        exit(EXIT_FAILURE);
    }

    fprintf(pipe, "%s", text);

    int status = pclose(pipe);

    if (status < 0) {
        perror("ERROR: with closing the pipe for pbcopy\n");
        exit(EXIT_FAILURE);
    }
}

void* manage_clip(void* args)
{
    data_info startClip =  run_get_clip_command();

    while (true) {
        data_info localClip = run_get_clip_command();

        if ((localClip.Data != startClip.Data) || (localClip.FileName != startClip.FileName)) {
            startClip = localClip;

            send_to_all_tcp(localClip);
        }
    }
}

