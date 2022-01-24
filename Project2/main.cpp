//
// Created by skillet on 21/01/2022.
//

#include "udpClient.h"
#include "RHP.h"

#define SERVER "137.112.41.87"
#define PORT 1874

#define MSG1 "hello"
#define MSG2 "hi"

void sendRHPcont(char* msg, struct sockaddr_in* pserver, int clientsock){
    struct RHP_payloadStruct sendRHP, recvRHP;
    uint8_t sendMsg[BUFSIZE], recvMsg[BUFSIZE];

    memset(&sendRHP, 0, sizeof(sendRHP));
    memset(&recvRHP, 0, sizeof(recvRHP));
    printf("RHP: sending RHP control message to server %s on port %d: \"%s\"\n", SERVER, PORT, msg);
    RHPstructFill(&sendRHP, msg);
    int msglen = RHPpack(&sendRHP, sendMsg);
    int checksumValid = 1;
    int i = 0;
    while(checksumValid != 0 && i < 10){
        i++;
        if(sendUDPmsg(clientsock, pserver, (char *)(sendMsg), msglen,(char *)(recvMsg)) != 0){
            continue;
        }
        checksumValid = RHPunpack(&recvRHP, recvMsg);
        if(checksumValid != 0){
            printf("retrying communication!\n");
        }
    }
}

int main(){
    struct sockaddr_in serverAddr;
    int clientSock;
    setupUDP(&serverAddr, &clientSock, SERVER, PORT);

    sendRHPcont(MSG1, &serverAddr, clientSock);
    sendRHPcont(MSG2, &serverAddr, clientSock);
    closeUDP(clientSock);
}