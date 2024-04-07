#include "../Notifications/Notify.h"
#include "../Clipboard/Clipboard.h"
#include "../Logging/Logging.h"
#include "Network.h"


#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <cstring>

#define UPD_PORT 8484
#define TCP_PORT 8484

#define BROADCAST_ADDRESS "255.255.255.255"
#define BUFFER_SIZE 1024

std::vector<std::string> ConnectedDevices;


void send_broadcast(const char *message)
{
    int socket_fd;
    struct sockaddr_in broadcast_addr = set_up_socket_udp(UPD_PORT, inet_addr(BROADCAST_ADDRESS), &socket_fd);

    int broadcast_enable = 1;
    int setOptions = setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

    if (setOptions < 0) {
        printf("Error: Setsockopt failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    int sendMessage = sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *) &broadcast_addr,
                             sizeof(broadcast_addr));

    if (sendMessage < 0) {
        printf("Error: Sendto failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("[UDP] LOCAL ADDRESS SENT: %s\n", message);

    logger("BROADCAST MESSAGE SENT", message);

    close(socket_fd);
}


void *recieve_broadcast(void *args)
{
    int socket_fd, binding;

    struct sockaddr_in client_address{};
    struct sockaddr_in server_address = set_up_socket_udp(UPD_PORT, INADDR_ANY, &socket_fd);

    // Binding socket to address
    binding = bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));

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
        ssize_t recv_len = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_address,
                                    &client_len);

        if (recv_len < 0) {
            printf("Error: receive failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        if (buffer != runGetClipCommand()) {
            runSetClipCommand(buffer);
            notifyDarwin("New clipboard", std::string(buffer));
        }

        printf("Received message from %s:%d: %s\n", inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port), buffer);

        const char* address = define_en0_interface();
        
        if (strcmp(address, buffer) != 0)
            ConnectedDevices.push_back(std::string(buffer));

        logger("[UDP} RECIEVED MESSAGE", buffer);
    }

    return NULL; // This code will never be executed, but required for pthread compilation
}


struct sockaddr_in set_up_socket_udp(int port, in_addr_t address, int *socket_fd)
{
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


char* define_en0_interface()
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    // Перебираем все сетевые интерфейсы
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        // Проверяем, что это интерфейс AF_INET (IPv4)
        if (family == AF_INET) {
            // Проверяем, что это интерфейс en0
            if (strcmp(ifa->ifa_name, "en0") == 0) {
                s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
                if (s != 0) {
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                    exit(EXIT_FAILURE);
                }

                printf("IP address for en0: %s\n", host);
                break;
            }
        }
    }

    freeifaddrs(ifaddr);

    return host;
}

void send_to_all_tcp(const char* message)
{
    for (size_t i = 0; i < ConnectedDevices.size(); ++i)
        send_to_device_tcp(message, ConnectedDevices[i].c_str());

}


void send_to_device_tcp(const char* message, const char* server_address)
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    size_t bytes_sent;

    // Создание TCP-сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Заполнение структуры адреса сервера
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);
    if (inet_pton(AF_INET, server_address, &serv_addr.sin_addr) <= 0) {
        perror("Некорректный адрес");
        exit(EXIT_FAILURE);
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Ошибка подключения");
        exit(EXIT_FAILURE);
    }

    FILE *file;

    if (findFile(message) != NULL)
    {
        file = fopen(findFile(message), "rb");
        // Файл найден, отправляем его содержимое
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            bytes_sent = send(sock, buffer, bytes_read, 0);
            if (bytes_sent != bytes_read) {
                perror("Ошибка отправки данных");
                exit(EXIT_FAILURE);
            }
        }
        fclose(file); // Закрытие файла
    } else {
        // Файл не найден, отправляем текстовое сообщение
        bytes_sent = send(sock, message, strlen(message), 0);
        if (bytes_sent != strlen(message)) {
            perror("Ошибка отправки данных");
            exit(EXIT_FAILURE);
        }
    }

    // Закрытие сокета
    close(sock);
}


void* set_up_tcp_server(void* args)
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

        // Принимаем данные от клиента
        FILE *received_file = fopen("received_file", "wb"); // Открываем файл на запись в бинарном режиме
        if (received_file == NULL) {
            perror("Ошибка при создании файла");
            exit(EXIT_FAILURE);
        }

        int bytes_received;
        while ((bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, bytes_received, received_file); // Записываем принятые данные в файл
        }
        fclose(received_file);
        printf("Файл успешно получен и сохранен\n");

        // Отправляем подтверждение клиенту
        char *message = "Сервер успешно получил файл";
        send(new_socket, message, strlen(message), 0);
        printf("Подтверждение отправлено клиенту\n");

        // Закрываем соединение
        close(new_socket);
    }
}