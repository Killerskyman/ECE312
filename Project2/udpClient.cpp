/************* UDP CLIENT CODE *******************/

#include "udpClient.h"

int setupUDP(struct sockaddr_in client, struct sockaddr_in server, int* clientSock, char* serverAddr, int port) {

    /*Create UDP socket*/
    if ((*clientSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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

    if (bind(*clientSock, (struct sockaddr *) &client, sizeof(client)) < 0) {
        perror("bind failed");
        return -1;
    }

    /* Configure settings in server address struct */
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(serverAddr);
    memset(server.sin_zero, '\0', sizeof server.sin_zero);
    return 0;
}

int sendUDPmsg(int clientSock, struct sockaddr_in* pserver, char* sendMsg, char* recvMsg) {
    struct sockaddr_in server = *pserver;
    /* send a message to the server */
    if (sendto(clientSock, sendMsg, strlen(sendMsg), 0,
               (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("sendto failed");
        return -1;
    }

    /* Receive message from server */
    recvfrom(clientSock, recvMsg, BUFSIZE, 0, NULL, NULL);

    printf("Received from server: %s\n", recvMsg);

    close(clientSock);
    return 0;
}