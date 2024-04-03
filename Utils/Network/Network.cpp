#include "../Notifications/Notify.h"
#include "../Clipboard/Clipboard.h"
#include "../Logging/Logging.h"
#include "Network.h"


#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


#define SEND_PORT 8787
#define RECEIVE_PORT 8484 // program can't send info to all users in network with different in/out ports
#define BROADCAST_ADDRESS "255.255.255.255"
#define BUFFER_SIZE 1024

int isFilePath(const char *path) {
    return access(path, F_OK) == 0;
}


void sendBroadcast(const char *message) {

    int socket_fd;
    struct sockaddr_in broadcast_addr = setUpUDPSocket(SEND_PORT, inet_addr(BROADCAST_ADDRESS), &socket_fd);

    int broadcast_enable = 1;
    int setOptions = setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

    if (setOptions < 0) {
        printf("Error: Setsockopt failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    int sendMessage = sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

    if (sendMessage < 0) {
        printf("Error: Sendto failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Broadcast message sent: %s\n", message);

    logger("BROADCAST MESSAGE SENT" , message);

    close(socket_fd);
}


void *receiveBroadcast(void *arg) {

    int socket_fd, binding;

    struct sockaddr_in client_address{};
    struct sockaddr_in server_address = setUpUDPSocket(SEND_PORT, INADDR_ANY, &socket_fd);

    // Binding socket to address
    binding = bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));

    if (binding < 0) {
        printf("Error: bind failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening for broadcasts...\n");

    while (1) {
        char buffer[BUFFER_SIZE] = {0};
        socklen_t client_len = sizeof(client_address);

        // Getting message from client
        ssize_t recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_len);

        if (recv_len < 0) {
            printf("Error: receive failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        if (buffer != runGetClipCommand()) {
            runSetClipCommand(buffer);
            notifyDarwin("New clipboard", std::string(buffer));
        }

        printf("Received message from %s:%d: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);

        logger("RECIEVED MESSAGE", buffer);
    }

    return NULL; // This code will never be executed, but required for pthread compilation
}


struct sockaddr_in setUpUDPSocket(int port, in_addr_t address, int *socket_fd) {

    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(socket_address));

    *socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (*socket_fd < 0) {
        printf("ERROR: socket creation");
        exit(EXIT_FAILURE);
    }

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = address;
    socket_address.sin_port = htons(port);

    return socket_address;
}