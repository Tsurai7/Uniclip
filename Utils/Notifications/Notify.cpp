#include <string>
#include <cstdlib>
#include <cstdio>

int notifyDarwin(std::string title, std::string message)
{

    std::string appleScriptCommand = "osascript -e 'display notification \"" + message + "\" with title \"" + title + "\" '";

    int result = system(appleScriptCommand.c_str());

    if (result == -1)
    {
        printf("Error: Failed to execute AppleScript.\n");
        return 1;
    }

    else
    {
        return 0;
    }
}
