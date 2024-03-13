#include "Network.h"

#define BROADCAST_PORT 8787
#define RECEIVE_PORT 8888
#define BROADCAST_ADDRESS "192.168.1.255"


void sendBroadcast(const char* message) {
    int socket_fd;
    struct sockaddr_in broadcast_address;

    // Создание сокета
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса для широковещательной рассылки
    memset(&broadcast_address, 0, sizeof(broadcast_address));
    broadcast_address.sin_family = AF_INET;
    broadcast_address.sin_port = htons(BROADCAST_PORT);
    if (inet_pton(AF_INET, BROADCAST_ADDRESS, &broadcast_address.sin_addr) <= 0) {
        perror("Invalid address");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    int broadcast_enable = 1;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("Setsockopt failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&broadcast_address, sizeof(broadcast_address)) == -1) {
        perror("Sendto failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Broadcast message sent: %s\n", message);
    sleep(1);
}


void receiveBroadcast(char** text) {
    int socket_fd;
    struct sockaddr_in server_address, client_address;

    // Создание сокета
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Настройка серверного адреса
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(RECEIVE_PORT);

    // Привязка сокета к адресу
    if (bind(socket_fd, (const struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening for broadcasts...\n");

    while (1) {
        char buffer[1024] = {0};
        socklen_t client_len = sizeof(client_address);

        // Получение сообщения от клиента
        ssize_t recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_len);

        if (recv_len < 0) {
            perror("Receive failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        *text = buffer;
        printf("Received message from %s:%d: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);
    }

    close(socket_fd);
}