#ifndef UNICLIP_NETWORK_H
#define UNICLIP_NETWORK_H

#include <sys/types.h>
#include <netinet/in.h>

void send_broadcast(const char *message);
void* receive_broadcast(void* args);
struct sockaddr_in set_up_udp_socket(int port, in_addr_t address, int *socket_fd);
void send_to_all_tcp(data_info info);
void* run_tcp_server(void* args);
void send_to_tcp_handler(data_info info, const char* server_address);
void send_file_to_tcp(data_info info, const char* server_address);
void send_text_to_tcp(const char* message, const char* server_address);
std::string receive_text_tcp(int socket);
void receive_file_tcp(int socket, const char* filename);
std::string get_ip_mac();
std::string get_ip_linux();
std::string get_ip_command();

#endif