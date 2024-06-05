#include <cstdio>
#include <cstdlib>
#include <unistd.h>

void Logger(const char* action, const char* message) {
    FILE *file;

    if (access("log.txt", F_OK) == -1) {
        file = fopen("log.txt", "w");

        if (file == NULL) {
            printf("Error opening log file");
            exit(1);
        }

        fclose(file);
    }

    file = fopen("log.txt", "a");

    if (file == NULL) {
        printf("Error opening log file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    fprintf(file, "");

    fprintf(file, "[%s %s] %s %s\n", __DATE__, __TIME__, action, message);

    fclose(file);
}