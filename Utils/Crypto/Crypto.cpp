#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "Crypto.h"


char* find(const char *dir_path, const char* entry_name, MessageType* info) {
    struct dirent** entries;

    int res = scandir(dir_path, &entries, NULL, NULL);

    if (res == -1) {
        printf("Error opening directory %s\n", dir_path);
        exit(1);
    }

    for (int i = 0; i < res; ++i) {
        struct dirent *entry = entries[i];

        if (entry->d_type == DT_DIR &&
            (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0))
            continue;

        char* new_path = (char*)calloc(strlen(dir_path) + strlen(entry->d_name) + 2, sizeof(char));
        strcpy(new_path, dir_path);
        strcat(new_path, "/");
        strcat(new_path, entry->d_name);
        //printf("%s", entry->d_name);

        if (entry->d_type == DT_DIR && strcmp(entry->d_name, entry_name) == 0) {
            char* new_path = (char*)calloc(strlen(dir_path) + strlen(entry->d_name) + 2, sizeof(char));
            strcpy(new_path, dir_path);
            strcat(new_path, "/");
            strcat(new_path, entry->d_name);
            (*info) = FILE_MESSAGE;
            return new_path;
        }

        if (entry->d_type == DT_DIR) {
            find(new_path, entry_name, info);
        }

        if (entry->d_type == DT_REG && strcmp(entry->d_name, entry_name) == 0) {
            char* new_path = (char*)calloc(strlen(dir_path) + strlen(entry->d_name) + 2, sizeof(char));
            strcpy(new_path, dir_path);
            strcat(new_path, "/");
            strcat(new_path, entry->d_name);
            (*info) = DIRECTORY_MESSAGE;
            return new_path;
        }

        free(entry);
        free(new_path);
    }

    free(entries);

    (*info) = TEXT_MESSAGE;
    return nullptr;
}