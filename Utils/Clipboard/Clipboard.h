#ifndef UNICLIP_CLIPBOARD_H
#define UNICLIP_CLIPBOARD_H

#include <iostream>

using namespace std;

string getClipWin();
string getClipLinux();
string getClipDarwin();

string runGetClipCommand();

#endif //UNICLIP_CLIPBOARD_H
