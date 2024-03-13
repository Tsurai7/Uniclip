#ifndef UNICLIP_NETWORK_H
#define UNICLIP_NETWORK_H

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

void sendBroadcast(char* message);
void receiveBroadcast();

#endif //UNICLIP_NETWORK_H