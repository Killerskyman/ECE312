//
// Created by skillet on 21/01/2022.
//

#ifndef PROJECT2_UDPCLIENT_H
#define PROJECT2_UDPCLIENT_H

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>      // From: https://linux.die.net/man/3/inet_addr
#include <cstring>
#include <unistd.h>

#define BUFSIZE 1024

extern int setupUDP(struct sockaddr_in client, struct sockaddr_in server, int* clientSock, char* serverAddr, int port);

extern int sendUDPmsg(int clientSock, struct sockaddr_in* pserver, char* sendMsg, char* recvMsg);

#endif //PROJECT2_UDPCLIENT_H
