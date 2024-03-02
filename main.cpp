#include <iostream>

using namespace std;

string getClipDarwin() {

    char buffer[128];
    std::string result;

    FILE *pipe = popen("pbpaste", "r");

    if (!pipe) {
        std::cerr << "Error opening pipe!" << std::endl;
        return "";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status == -1) {
        std::cerr << "Error closing pipe!" << std::endl;
        return "";
    }

    return result;
}


int main() {
    string test = getClipDarwin();
    cout << "res:" << test << endl;
    return 0;
}
