#include <string>
#include <cstdlib>
#include <cstdio>

int Notify(std::string title, std::string message)
{
#ifdef __APPLE__
        std::string appleScriptCommand = "osascript -e 'display notification \"" + message + "\" with title \"" + title + "\" '";

        int result = system(appleScriptCommand.c_str());

        if (result == -1) {
            printf("Error: Failed to execute AppleScript.\n");
            exit(EXIT_FAILURE);
        }
#endif

    return 0;
}