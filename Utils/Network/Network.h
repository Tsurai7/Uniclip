#ifndef UNICLIP_NETWORK_H
#define UNICLIP_NETWORK_H

#include <sys/types.h>
#include <vector>


struct sockaddr_in setUpUDPSocket(int port, in_addr_t address, int *socket_fd);
void sendBroadcast(const char* message);
void *receiveBroadcast(void *arg);

char* defineLocalEn0Interface();

void sendInfoTcp(const char* message, const char* server_address);
void sendInfoToAllTcp(const char* message);
void *setUpTCPServer(void* args);
#endif