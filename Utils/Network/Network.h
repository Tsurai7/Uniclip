#ifndef UNICLIP_NETWORK_H
#define UNICLIP_NETWORK_H

#include <sys/types.h>
#include <netinet/in.h>
#include <unordered_set>

void send_broadcast(const char *message);

void *recieve_broadcast(void *args);

struct sockaddr_in set_up_socket_udp(int port, in_addr_t address, int *socket_fd);

void send_to_all_tcp(const char* message);

void send_to_device_tcp(const char* message, const char* server_address);

void* set_up_tcp_server(void* args);

std::string get_ip_mac();

std::string get_ip_linux();

std::string get_ip_command();


#endif