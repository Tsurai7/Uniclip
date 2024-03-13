#include <iostream>

#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Server/Server.h"
#include "Utils/Client/Client.h"

using namespace std;


int main(int argc, char* argv[]) {
    cout << runGetClipCommand() << endl;

    return 0;
}
