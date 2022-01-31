/*
 * udpClient.cpp
 * provides functionality for sending and recieving UDP messages
 *
 * setupUDP() - setups a udp connectiong by initializing the given values
 *
 * sendUDP() - sends and recieves a UDP message from the server
 *
 * NOTE: function descriptions in header file
 *
 * authors - Skyler Cleland, Justin Benson
 * last revision: 2022-01-24
 */

#include <malloc.h>
#include "udpClient.h"

int setupUDP(struct sockaddr_in* server, SOCKET* clientSock, char* serverAddr, int port) {

    struct sockaddr_in client;                  //used for getting client socket

#if WIN32
    WSADATA winData;
    WSAStartup(MAKEWORD(2,2), &winData);
#endif

    /*Create UDP socket*/
    if ((*clientSock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        perror("cannot create socket");
        return -1;
    }

    /* Bind to an arbitrary return address.
     * Because this is the client side, we don't care about the address
     * since no application will initiate communication here - it will
     * just send responses
     * INADDR_ANY is the IP address and 0 is the port (allow OS to select port)
     * htonl converts a long integer (e.g. address) to a network representation
     * htons converts a short integer (e.g. port) to a network representation */
    memset((char *) &client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = htons(0);

    if (bind(*clientSock, (const sockaddr *)(&client), sizeof(client)) < 0) {
        perror("bind failed");
        return -1;
    }

    /* Configure settings in server address struct */
    memset((char *) server, 0, sizeof(*server));
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    server->sin_addr.s_addr = inet_addr(serverAddr);
    memset(server->sin_zero, '\0', sizeof server->sin_zero);
    return 0;
}

int sendUDPmsg(SOCKET clientSock, struct sockaddr_in* pserver, char* sendMsg, int sendmsglen, char* recvMsg) {

    /* send a message to the server */
    if (sendto(clientSock, sendMsg, sendmsglen, 0,(struct sockaddr *) pserver, sizeof(*pserver)) < 0) {
        perror("sendto failed");
        return -1;
    }

    /* Receive message from server */
    recvfrom(clientSock, recvMsg, BUFSIZE, 0, nullptr, nullptr);

    printf("Received from server\n");

    return 0;
}

void closeUDP(SOCKET clientSock){
    closesocket(clientSock);
#if WIN32
    WSACleanup();
#endif
}