#ifndef UNICLIP_CLIPBOARD_H
#define UNICLIP_CLIPBOARD_H

#include <string>

std::string getClipCommand(const char* command);
void setClipCommand(const char* command, const char* text);

std::string runGetClipCommand();
void runSetClipCommand(const char* text);

#endif
