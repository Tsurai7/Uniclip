#include "../Clipboard/Clipboard.h"
#include "../Notifications/Notify.h"
#include "../Crypto/Crypto.h"
#include "Network.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <cstring>
#include <net/if.h>
#include <unordered_set>

#define UPD_PORT 108484
#define TCP_PORT 108787

#define BROADCAST_ADDRESS "255.255.255.255"
#define BUFFER_SIZE (1024 * 64)
#define PATH_MAX 256

std::unordered_set<std::string> ConnectedDevices;

const int p = 61;
const int q = 53;

int publicKey, privateKey, n;

void send_broadcast(const char *message)
{
    ConnectedDevices.emplace(get_ip_command());
    int socket_fd;
    struct sockaddr_in broadcast_addr = set_up_udp_socket(UPD_PORT, inet_addr(BROADCAST_ADDRESS), &socket_fd);

    int broadcast_enable = 1;
    int setOptions = setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

    if (setOptions < 0) {
        perror("Error: Setsockopt failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    int sendMessage = sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *) &broadcast_addr,
            sizeof(broadcast_addr));

    if (sendMessage < 0) {
        perror("Error: Sendto failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[UDP] LOCAL ADDRESS SENT: %s\n", message);

    close(socket_fd);
}

void *recieve_broadcast(void *args) {
    int socket_fd, binding;

    struct sockaddr_in client_address{};
    struct sockaddr_in server_address = set_up_udp_socket(UPD_PORT, INADDR_ANY, &socket_fd);

    binding = bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));

    if (binding < 0) {
        perror("Error binding to socket");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[UDP] Listening for broadcasts on port: %d\n", UPD_PORT);

    while (true) {
        char buffer[BUFFER_SIZE] = {0};
        socklen_t client_len = sizeof(client_address);

        ssize_t recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_address, &client_len);

        if (recv_len < 0) {
            perror("Error: receive failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        printf("[UDP] RECIEVED MESSAGE %s:%d: %s\n", inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port), buffer);


        if (ConnectedDevices.find(std::string( inet_ntoa(client_address.sin_addr))) == ConnectedDevices.end()
            && get_ip_command() != std::string(buffer)) {
            ConnectedDevices.emplace(std::string( inet_ntoa(client_address.sin_addr)));
            send_broadcast(get_ip_command().c_str());
        }
    }
}

struct sockaddr_in set_up_udp_socket(int port, in_addr_t address, int *socket_fd) {
    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(socket_address));

    *socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (*socket_fd < 0) {
        perror("ERROR: socket creation");
        exit(EXIT_FAILURE);
    }

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = address;
    socket_address.sin_port = htons(port);

    return socket_address;
}

std::string get_ip_linux() {
    std::string localIpAddress;
    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* ifa = nullptr;
    void* tmpAddrPtr = nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET && !(ifa->ifa_flags & IFF_LOOPBACK)) {
            tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            localIpAddress = std::string(addressBuffer);
            break;
        }
    }

    if (ifAddrStruct != nullptr)
        freeifaddrs(ifAddrStruct);

    return localIpAddress;
}

std::string get_ip_mac() {
    std::string ipAddress;
    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* ifa = nullptr;
    void* tmpAddrPtr = nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "en0") == 0) {
            tmpAddrPtr = &reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            ipAddress = std::string(addressBuffer);
            break;
        }
    }

    if (ifAddrStruct != nullptr)
        freeifaddrs(ifAddrStruct);

    return ipAddress;
}

std::string get_ip_command() {
#ifdef __APPLE__
    return get_ip_mac();
#elif __linux__
    return get_ip_linux();
#endif
}

void send_to_all_tcp(data_info info) {
    for (std::string element : ConnectedDevices)
        send_to_tcp_handler(info, element.c_str());
}

void send_text_to_tcp(const char* message, const char* server_address) {
    generateRSAKeys(p, q, publicKey, privateKey, n);

    std::string encryptedMessage = encryptRSA(message, publicKey, n);
    printf("Encrypted message: %s\n", encryptedMessage.c_str());

    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERROR: socket creation");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);

    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        perror("Incorrect address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (send(sock, "[TEXT]", strlen("[TEXT]"), 0) < 0) {
        perror("Sending error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    sleep(1);

    if (send(sock, encryptedMessage.c_str(), strlen(encryptedMessage.c_str()), 0) < 0) {
        perror("Message sending error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Message sent: %s\n", encryptedMessage.c_str());

    close(sock);
}

void send_file_to_tcp(data_info info, const char* server_address) {
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);
    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        perror("Incorrect address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (send(sock, "[FILE]", strlen("[FILE]"), 0) < 0) {
        perror("Sending file error");
        close(sock);
        exit(EXIT_FAILURE);
    }
    sleep(1);

    if (send(sock, info.FileName.c_str(), strlen(info.FileName.c_str()), 0) < 0) {
        perror("Sending file name error");
        close(sock);
        exit(EXIT_FAILURE);
    }
    sleep(1);

    FILE* file = fopen(info.FilePath.c_str(), "rb");
    if (!file) {
        perror("Opening file error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    rewind(file);

    auto buffer = (char*)malloc(file_size);

    if (!buffer) {
        perror("Memory allocation error");
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);

    if (bytes_read != file_size) {
        perror("Reading file error");
        free(buffer);
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (send(sock, buffer, file_size, 0) != file_size) {
        perror("Sending data error");
        free(buffer);
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("File sent\n");
    free(buffer);
    fclose(file);
    close(sock);
}

void send_to_tcp_handler(data_info info, const char* server_address) {
    switch (info.Type) {
        case Text:
            send_text_to_tcp(info.Data.c_str(), server_address);
            break;
        case File:
            send_file_to_tcp(info, server_address);
            break;
        default:
            perror("Error: unknown message type\n");
            break;
    }
}

std::string recieve_text_tcp(int socket) {
    char text[BUFFER_SIZE];
    memset(text, 0, BUFFER_SIZE);

    int valread = read(socket, text, BUFFER_SIZE);

    std::string decryptedMessage = decryptRSA(std::string(text), privateKey, n);

    run_set_clip_command(decryptedMessage.c_str());
    printf("Recieved text: %s\n", text);

    printf("Decrypted text: %s\n", decryptedMessage.c_str());

    notify("Uniclip", "New local clip");

    return text;
}

void recieve_file_tcp(int socket, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t total_bytes_received = 0;

    while (true) {
        ssize_t bytes_received = read(socket, buffer, BUFFER_SIZE);
        if (bytes_received < 0) {
            perror("Error receiving data");
            fclose(file);
            exit(1);
        } else if (bytes_received == 0) {
            // End of file
            break;
        }

        size_t bytes_written = fwrite(buffer, 1, bytes_received, file);
        if (bytes_written != (size_t)bytes_received) {
            perror("Error writing to file");
            fclose(file);
            exit(1);
        }

        total_bytes_received += bytes_received;
    }

    fclose(file);
    printf("File successfully saved (%lu bytes)\n", total_bytes_received);

    char resolved_path[PATH_MAX];
    if (realpath(filename, resolved_path) == NULL) {
        perror("Error getting file path");
        exit(EXIT_FAILURE);
    }

    run_set_clip_command(resolved_path);
}

void* run_tcp_server(void* args) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Error connecting to socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Error listening the socket ");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started on port:  %d\n", TCP_PORT);

    while (true) {
        printf("Waiting for incoming connections...\n");

        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Establishing connection error");
            continue;
        }

        printf("New connection established\n");

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        printf("Header: %s\n", buffer);

        if (strstr(buffer, "[TEXT]") != NULL) {
            recieve_text_tcp(new_socket);
        }

        else if (strstr(buffer, "[FILE]") != NULL) {
            int newVal = read(new_socket, buffer, BUFFER_SIZE);

            std::string name = buffer;
            printf("File name: %s\n", buffer);
            recieve_file_tcp(new_socket, name.c_str());

            notify("Uniclip", "New local clip");
        }

         else {
            perror("Unknown message type");
        }

        close(new_socket);
    }
}
