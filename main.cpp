#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

string getClipWin();
string getClipLinux();
string getClipDarwin();
string runGetClipCommand();

string runGetClipCommand() {
    #ifdef __APPLE__
        return getClipDarwin();
    #elif __linux__
        return getClipLinux();
    #elif _WIN32
        return getClipWin();
    #else
        return "Unknown operating system.\n";
    #endif
}

string getClipLinux() {

    char buffer[1024];
    string result;

    FILE* pipe = popen("xclip -o", "r");

    if (!pipe) {
        return "Error opening pipe!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status == -1) {
        return "Error closing pipe!" ;
    }

    return result;
}

string getClipDarwin() {

    char buffer[1024];
    string result;

    FILE *pipe = popen("pbpaste", "r");

    if (!pipe) {
        return "Error opening pipe!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status == -1) {
        return "Error closing pipe!";
    }

    return result;
}

string getClipWin() {
    char buffer[1024];
    string result;

    FILE* pipe = popen("powershell.exe -command Get-clipboard", "r");

    if (!pipe) {
        return "Error opening pipe!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    int status = pclose(pipe);

    if (status == -1) {
        return "Error closing pipe!";
    }

    return result;
}


int main() {

    /* int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    char* ip = "172.253.122.139";
    struct sockaddr_in address;
    address.sin_port = htons(80);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr.s_addr);


    int result = connect(socketFD, reinterpret_cast<const sockaddr *>(&address), sizeof address);

    if (result == 0) {
        printf("Connection was succesfull");
    }

    char* message;
    message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";
    send(socketFD, message, strlen(message), 0);

    char buffer[1024];
    recv(socketFD, buffer, 1024, 0);

    printf("Response was: %s\n", buffer); */






    cout << runGetClipCommand() << endl;
    return 0;
}
