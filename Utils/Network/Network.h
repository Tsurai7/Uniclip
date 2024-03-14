#ifndef UNICLIP_NETWORK_H
#define UNICLIP_NETWORK_H

#include <sys/types.h>


struct sockaddr_in setUpUDPSocket(int port, in_addr_t address, int *socket_fd);
void sendBroadcast(const char* message);
void *receiveBroadcast(void *arg);

#endif