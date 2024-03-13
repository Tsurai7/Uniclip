#ifndef UNICLIP_NETWORK_H
#define UNICLIP_NETWORK_H

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>


void sendBroadcast(const char* message);
void receiveBroadcast();

#endif //UNICLIP_NETWORK_H