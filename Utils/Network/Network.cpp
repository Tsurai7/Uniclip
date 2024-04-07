#include "../Notifications/Notify.h"
#include "../Clipboard/Clipboard.h"
#include "../Logging/Logging.h"
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


        if (ConnectedDevices.find(std::string(buffer)) == ConnectedDevices.end() && get_ip_command() != std::string(buffer))
        {
            ConnectedDevices.emplace(std::string(buffer));
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

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
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


        // IPv4 and interface name "en0"
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

void send_to_all_tcp(const char* message)
{
    for (std::string element : ConnectedDevices)
        send_to_device_tcp(message, element.c_str());
}


void send_to_device_tcp(const char* message, const char* server_address)
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Создание TCP-сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка при создании сокета");
        return;
    }

    // Заполнение структуры адреса сервера
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);
    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        perror("Некорректный адрес");
        close(sock);
        return;
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Ошибка подключения");
        close(sock);
        return;
    }

    // Отправка сообщения на сервер
    if (send(sock, message, strlen(message), 0) < 0) {
        perror("Ошибка отправки сообщения");
        close(sock);
        return;
    }

    printf("Сообщение отправлено: %s\n", message);

    // Закрытие сокета
    close(sock);
}


void* run_tcp_server(void *args)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // Создаем TCP сокет
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Заполняем структуру адреса
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_PORT);

    // Связываем сокет с локальным адресом
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Ошибка при привязке сокета");
        exit(EXIT_FAILURE);
    }

    // Переводим сокет в режим прослушивания
    if (listen(server_fd, 3) < 0) {
        perror("Ошибка при прослушивании");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", TCP_PORT);

    while (1) {
        printf("Ожидание входящих соединений...\n");

        // Принимаем входящее соединение
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Ошибка при приеме соединения");
            exit(EXIT_FAILURE);
        }

        printf("Новое соединение установлено\n");

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        printf("Полученные данные: %s\n", buffer);

        // Закрываем соединение
        close(new_socket);
    }

    // Закрываем серверный сокет
    close(server_fd);
}