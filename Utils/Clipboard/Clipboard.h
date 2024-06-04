#ifndef UNICLIP_CLIPBOARD_H
#define UNICLIP_CLIPBOARD_H

#include <string>

enum data_type {
    File,
    Text
};

struct data_info {
    data_type Type;
    std::string FileName;
    std::string FilePath;
    std::string Data;
};

data_info get_clip_command(const char* command);
data_info run_get_clip_command();
void set_clip_command(const char* command, const char* text);
void run_set_clip_command(const char* text);
void* manage_clip(void* args);

#endif