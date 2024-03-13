#ifndef UNICLIP_CLIPBOARD_H
#define UNICLIP_CLIPBOARD_H

#include <iostream>

using namespace std;

string getClipCommand(const char* command);
void setClipCommand(const char* command, string text);

string runGetClipCommand();
void runSetClipCommand(char* text);

#endif
