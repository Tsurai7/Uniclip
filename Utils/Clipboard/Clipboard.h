#ifndef UNICLIP_CLIPBOARD_H
#define UNICLIP_CLIPBOARD_H

#include <string>

std::string getClipCommand(const char* command);
void setClipCommand(const char* command, std::string text);

std::string runGetClipCommand();
void runSetClipCommand(char* text);

#endif
