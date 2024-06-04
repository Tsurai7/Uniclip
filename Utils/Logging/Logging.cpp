#include <cstdio>
#include <ctime>
#include <unistd.h>

void Logger(const char* action, const char* message)
{
    FILE* file;
    const char* log_file_path = "log.txt";

    if (access(log_file_path, F_OK) == -1) {

        file = fopen(log_file_path, "w");

        if (file == NULL) {
            fprintf(stderr, "Error opening log file: %s\n", log_file_path);
            return;
        }
        fclose(file);
    }

    file = fopen(log_file_path, "a");

    if (file == NULL) {
        fprintf(stderr, "Error opening log file: %s\n", log_file_path);
        return;
    }

    time_t now = time(nullptr);
    struct tm* local_time = localtime(&now);
    char date_time[64];

    strftime(date_time, sizeof(date_time), "%Y-%m-%d %H:%M:%S", local_time);

    fprintf(file, "\n\n[%s] %s\n%s\n", date_time, action, message);
    fclose(file);
}