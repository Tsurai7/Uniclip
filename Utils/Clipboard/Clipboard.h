#ifndef UNICLIP_CLIPBOARD_H
#define UNICLIP_CLIPBOARD_H

#include <string>


std::string get_clip_command(const char* command);
void set_clip_command(const char* command, const char* text);

std::string run_get_clip_command();
void run_set_clip_command(const char* text);

void* manage_clip(void* arg);

#endif
