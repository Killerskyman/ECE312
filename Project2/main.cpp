/*
 * main.cpp
 * runs an example of the RHP protocol over UDP to the specified server and port
 *
 * main() - sets up variables needed by other functions and calls the setup and send functions
 *
 * sendRHPcont() - a helper function for main that sets up the UDP connection and RHP message and provides logic on what to do with the RHP message
 *
 * authors - Skyler Cleland, Justin Benson
 * last revision: 2022-01-24
 */

#include <cstdlib>
#include "udpClient.h"
#include "RHP.h"
#include "RHMP.h"

#define SERVER "localhost"          //server to send the RHP message to
#define PORT 1874                       //port on server to send RHP message

#define MSG1 "hello"                    //message 1
#define MSG2 "hi"                       //message 2


/*
 * helper function for main to easily send multiple strings over the RHP control message protocal
 *
 * sendmsg - string to send
 * pserver - pointer to the struct sockaddr_in that is for the server
 * clientsock - the socket to use to send/recieve the RHP message
 *
 * please call the setupUDP() function before calling this function as that provides the server and client socket
 */
int sendRHPoverUDP(RHP_TYPE rhpType, char* sendmsg, int dataLen, char* recvmsg, struct sockaddr_in* pserver, SOCKET clientsock){
    struct RHP_payloadStruct sendRHP, recvRHP;              //structs for the send/recv RHP messages
    uint8_t sendMsg[BUFSIZE], recvMsg[BUFSIZE];             //buffers for the send/recv UDP messages

    memset(&sendRHP, 0, sizeof(sendRHP));           //null out the RHP message structs
    memset(&recvRHP, 0, sizeof(recvRHP));
    printf("RHP: sending RHP message to server %s on port %d: \"%s\"\n", SERVER, PORT, sendmsg);    //debug to console
    RHPstructFill(rhpType, &sendRHP, sendmsg, dataLen);       //fill in the structures using the nominal values for a control message
    int msglen = RHPpack(&sendRHP, sendMsg);    //pack the sendMsg buffer for RHP using the struct
    int checksumValid = 1;                                           //setup checksum validation
    int i = 0;                                                      //used to keep track of how many times we try to send to the server
    while(checksumValid != 0 && i < 10){                            //try sending the message at most 10 times or until the checksum is valid
        i++;                                                        //increase try count
        if(sendUDPmsg(clientsock, pserver, (char *)(sendMsg), msglen,(char *)(recvMsg)) != 0){  //send the UDP message and recvieve and store the server udp message to recvMsg
            continue;       //if failure occurs don't try to unpack or validate sum, continue to next try
        }
        checksumValid = RHPunpack(&recvRHP, recvMsg);   //unpack the udp message and check the checksum
        if(checksumValid != 0){//0 is success and anything else is failure
            printf("retrying communication!\n");//if the checksum is not valid, print that the program will retry communication
        }
    }// while loop exited upon checksum validation or the number of trys is greater than 10
    if(recvmsg != nullptr) memcpy(recvmsg, recvRHP.payload, recvRHP.length);
    return recvRHP.type;
}

void sendRHMPoverRHP(RHMP_TYPE rhmpType, char* msg, struct sockaddr_in* pserver, SOCKET clientsock){
    struct RHMP_payloadStruct sendRHMP, recvRHMP;
    uint8_t sendMsg[BUFSIZE], recvMsg[BUFSIZE];
    RHMP_messageFill(rhmpType, &sendRHMP, msg);
    int msglen = RHMP_pack(&sendRHMP, sendMsg);
    int recvType = sendRHPoverUDP(RHMP, (char *)sendMsg, msglen, (char*)(recvMsg), pserver, clientsock);
    if(recvType != RHMP) return;
    RHMP_unpack(&recvRHMP, (uint8_t *) recvMsg);
}


/*
 * ties everything together and runs things in the proper order
 */
int main(){
    struct sockaddr_in serverAddr;                                                      //struct for storing the server
    SOCKET clientSock;                                                                     //stores the client socket
    setupUDP(&serverAddr, &clientSock, SERVER, PORT);     //setup UDP connection

    sendRHPoverUDP(COMMAND, MSG1, sizeof(MSG1), nullptr, &serverAddr, clientSock);                 //send message 1
    sendRHPoverUDP(COMMAND, MSG2, sizeof(MSG2), nullptr, &serverAddr, clientSock);                 //send message 2

    sendRHMPoverRHP(MESSAGE_REQUEST, nullptr, &serverAddr, clientSock);
    sendRHMPoverRHP(ID_REQUEST, nullptr, &serverAddr, clientSock);

    closeUDP(clientSock);                                                           //close the socket
    exit(0);
}