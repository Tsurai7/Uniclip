#ifndef UNICLIP_CLIPBOARD_H
#define UNICLIP_CLIPBOARD_H

#include <string>


std::string getClipCommand(const char* command);
void setClipCommand(const char* command, const char* text);
const char* findFile(const char* fileName);
std::string runGetClipCommand();
void runSetClipCommand(const char* text);
void* manageClip(void* arg);

#endif
