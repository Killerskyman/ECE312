/*
 * udpClient.h
 * provides functionality for sending and recieving UDP messages
 *
 * setupUDP() - setups a udp connectiong by initializing the given values
 *
 * sendUDP() - sends and recieves a UDP message from the server
 *
 * authors - Skyler Cleland, Justin Benson
 * last revision: 2022-01-24
 */

#ifndef PROJECT2_UDPCLIENT_H
#define PROJECT2_UDPCLIENT_H

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>      // From: https://linux.die.net/man/3/inet_addr
#include <cstring>
#include <unistd.h>

#define BUFSIZE 1024            //max buffer size for sending and recieving

/*
 * setupUDP - used to setup the server sockaddr_in and client socket based on a given server address and port
 *
 * server - sockaddr_in to initialize
 * clientSock - returns the FD of the socket it bound to
 * serverAddr - address of the server you wish to setup the connection to
 * port - port on which to communicate over
 *
 * return - returns success, 0 or failure, not 0
 */
extern int setupUDP(struct sockaddr_in* server, int* clientSock, char* serverAddr, int port);

/*
 * sendUDPmsg - it sends and then recieves a UDP message to the selected server and socket
 *
 * clientSock - socket to send UDP message over
 * pserver - server struct to send message to
 * sendMsg - message to send
 * sendmsglen - length of the message, makes it so it doesn't have to use strlen to figure out message length (good for messages with 0x00 characters)
 * recvMsg - message that was recieved after the your message was sent
 *
 * return - returns success, 0 or failure, not 0
 */
extern int sendUDPmsg(int clientSock, struct sockaddr_in* pserver, char* sendMsg, int sendmsglen, char* recvMsg);

/*
 * used to close the socket once UDP communication is done, useful when using the socket multiple times.
 *
 * clientSock - socket to close
 */
extern void closeUDP(int clientSock);

#endif //PROJECT2_UDPCLIENT_H
