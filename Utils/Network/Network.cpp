#include "../Clipboard/Clipboard.h"
#include "Network.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unordered_set>

#define UPD_PORT 8484
#define TCP_PORT 8787

#define BROADCAST_ADDRESS "255.255.255.255"
#define BUFFER_SIZE 1024

std::unordered_set<std::string> ConnectedDevices;


void send_broadcast(const char *message)
{
    int socket_fd;
    struct sockaddr_in broadcast_addr = set_up_udp_socket(UPD_PORT, inet_addr(BROADCAST_ADDRESS), &socket_fd);

    int broadcast_enable = 1;
    int setOptions = setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

    if (setOptions < 0)
    {
        printf("Error: Setsockopt failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    int sendMessage = sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *) &broadcast_addr,
                             sizeof(broadcast_addr));

    if (sendMessage < 0)
    {
        printf("Error: Sendto failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[UDP] LOCAL ADDRESS SENT: %s\n", message);

    close(socket_fd);
}

void *recieve_broadcast(void *args)
{
    //ConnectedDevices.emplace("127.0.0.1");
    int socket_fd, binding;

    struct sockaddr_in client_address{};
    struct sockaddr_in server_address = set_up_udp_socket(UPD_PORT, INADDR_ANY, &socket_fd);

    // Binding socket to address
    binding = bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));

    if (binding < 0)
    {
        printf("Error: bind failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[UDP] Listening for broadcasts...\n");

    while (true)
    {
        char buffer[BUFFER_SIZE] = {0};
        socklen_t client_len = sizeof(client_address);

        // Getting message from client
        ssize_t recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_address,
                                    &client_len);

        if (recv_len < 0)
        {
            printf("Error: receive failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        printf("[UDP] RECIEVED MESSAGE %s:%d: %s\n", inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port), buffer);


        if (ConnectedDevices.find(std::string( inet_ntoa(client_address.sin_addr))) == ConnectedDevices.end() && get_ip_command() != std::string(buffer))
        {
            ConnectedDevices.emplace(std::string( inet_ntoa(client_address.sin_addr)));
            send_broadcast(get_ip_command().c_str());
        }
    }
}


struct sockaddr_in set_up_udp_socket(int port, in_addr_t address, int *socket_fd)
{
    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(socket_address));

    *socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (*socket_fd < 0)
    {
        printf("ERROR: socket creation");
        exit(EXIT_FAILURE);
    }

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = address;
    socket_address.sin_port = htons(port);

    return socket_address;
}


std::string get_ip_linux()
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}


std::string get_ip_mac()
{
    std::string ipAddress;
    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* ifa = nullptr;
    void* tmpAddrPtr = nullptr;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "en0") == 0)
        {
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


void send_to_all_tcp(data_info info)
{
    ConnectedDevices.emplace("127.0.0.1");
    for (std::string element : ConnectedDevices)
        send_to_tcp_handler(info, element.c_str());
}


struct sockaddr_in set_up_tcp_socket(int port, in_addr_t address, int *socket_fd)
{
    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(socket_address));

    *socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (*socket_fd < 0)
    {
        printf("ERROR: socket creation");
        exit(EXIT_FAILURE);
    }

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = address;
    socket_address.sin_port = htons(port);

    return socket_address;
}


void send_text_to_tcp(const char* message, const char* server_address)
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка при создании сокета");
        return;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);

    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        printf("Incorrect address");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection error");
        close(sock);
        return;
    }

    if (send(sock, "[TEXT]", strlen("[TEXT]"), 0) < 0) {
        perror("Sending error");
        close(sock);
        return;
    }

    sleep(1);

    if (send(sock, message, strlen(message), 0) < 0) {
        perror("Ошибка отправки сообщения");
        close(sock);
        return;
    }

    printf("Message sent: %s\n", message);

    close(sock);
}


void send_file_to_tcp(data_info info, const char* server_address)
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        return;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);

    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        perror("Некорректный адрес");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Ошибка подключения");
        close(sock);
        return;
    }

    if (send(sock, "[FILE]", strlen("[FILE]"), 0) < 0) {
        perror("Ошибка отправки сообщения");
        close(sock);
        return;
    }

    sleep(1);

    if (send(sock, info.FileName.c_str(), strlen(info.FileName.c_str()), 0) < 0) {
        perror("Ошибка отправки сообщения");
        close(sock);
        return;
    }

    sleep(1);

    FILE* file = fopen(info.FilePath.c_str(), "rb");

    if (!file) {
        perror("Ошибка открытия файла");
        close(sock);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) != bytes_read) {
            perror("Ошибка отправки данных");
            close(sock);
            fclose(file);
            return;
        }
    }

    printf("File sent\n");

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


std::string recieve_text_tcp(int socket)
{
    char text[BUFFER_SIZE];
    memset(text, 0, BUFFER_SIZE);

    int valread = read(socket, text, BUFFER_SIZE);

    printf("Полученный текст: %s\n", text);

    return text;
}


void recieve_file_tcp(int socket, const char* filename)
{
    FILE* file = fopen(filename, "wb");

    if (file == NULL) {
        perror("Ошибка при открытии файла");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    do {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = read(socket, buffer, BUFFER_SIZE);
        if (bytes_received < 0) {
            perror("Ошибка при чтении данных из сокета");
            fclose(file);
            exit(1);
        }
        fwrite(buffer, 1, bytes_received, file);
    } while (bytes_received == BUFFER_SIZE);

    fclose(file);
    printf("Файл успешно сохранен\n");


    char resolved_path[PATH_MAX];

    if (realpath(filename, resolved_path) == NULL) {
        perror("Ошибка при получении пути к файлу");
        exit(1);
    }

    run_set_clip_command(resolved_path);
}


void* run_tcp_server(void* args)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Ошибка при создании сокета");
        return NULL;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Ошибка при привязке сокета");
        close(server_fd);
        return NULL;
    }

    if (listen(server_fd, 3) < 0) {
        perror("Ошибка при прослушивании");
        close(server_fd);
        return NULL;
    }

    printf("Сервер запущен на порту %d\n", TCP_PORT);

    while (true) {
        printf("Ожидание входящих соединений...\n");

        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Ошибка при приеме соединения");
            continue;
        }

        printf("Новое соединение установлено\n");

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        printf("Заголовок: %s\n", buffer);

        if (strstr(buffer, "[TEXT]") != NULL) {
            recieve_text_tcp(new_socket);
        }
        else if (strstr(buffer, "[FILE]") != NULL) {
            int newVal = read(new_socket, buffer, BUFFER_SIZE);

            std::string name = buffer;
            printf("Имя файла: %s\n", buffer);
            recieve_file_tcp(new_socket, name.c_str());
        }
         else {
            printf("Неизвестный тип сообщения: %s\n", buffer);
        }

        close(new_socket);
    }
}
