// From page 58, Figure 2.14 of Stallings

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <unistd.h>

//#define LOCALSERVER

#define SERVERPATH "/home/skillet/git/ECE312/project1 - sockets/ThreadServer"
#define SERVERPAHTLEN 70

#define PORTNUM 2001
#define CLIENTSTARTDELAY 1
#define SENDDATALEN 255
#define USERNAMELEN 30
#define MESSAGELEN SENDDATALEN-USERNAMELEN
#define SERVERNAMELEN 255

#define GETHOSTNAMERBUF 4096

void error(char *msg) {
    perror(msg);
    exit(0);
}

void * startServer(void * portno) {
    char serverStart[70];
    snprintf(serverStart, SERVERPAHTLEN, "\"%s\" %d", SERVERPATH, *((int*)portno));
    system(serverStart);
}

int initServer(pthread_t* serverThread, int portno){
    int ret;
    ret = pthread_create(serverThread, NULL, startServer, (void *) &portno);
    if (ret) {
        printf("ERROR: Return Code from pthread_create() is %d\n", ret);
        error("ERROR creating thread");
    }
    sleep(CLIENTSTARTDELAY);
    return ret;
}

struct initStruct{
    char* username;
    int sockfd;
    pthread_t* otherThread;
};

void * sendData(void * data){
    char buffer[SENDDATALEN];
    struct initStruct* initData;
    initData = data;
    char* username = initData->username;
    int sockfd = initData->sockfd;
    pthread_t recvThread = *initData->otherThread;
    int n;
    int done = 0;
    char msg[MESSAGELEN];

    while(done == 0) {
        bzero(msg, MESSAGELEN);
        bzero(buffer, SENDDATALEN);
//        printf("<%s> ", username);
        fgets(msg, MESSAGELEN, stdin);
        msg[strcspn(msg, "\r\n")] = 0;
        if(strcmp(msg, "quit")==0){
            printf("closing the program\n");
            snprintf(buffer, SENDDATALEN, "%s\n", msg);
            done = 1;
        }else {
            snprintf(buffer, SENDDATALEN, "<%s> %s\n", username, msg);
        }
        n = send(sockfd, buffer, SENDDATALEN,0);
        if (n < 0) {
            error("ERROR: sending message failed\n");
        }
    }
    printf("exiting send thread\n");
    pthread_exit(NULL);
}

void * recieveData(void * data){
    struct initStruct* initData;
    initData = data;
    char buffer[SENDDATALEN];
    int sockfd = initData->sockfd;
    pthread_t sendThread = *initData->otherThread;
    int n;
    int done = 0;

    while(done == 0){
        bzero(buffer, SENDDATALEN);
        n = recv(sockfd, buffer, SENDDATALEN,0);
        if(n < 0){
            error("ERROR: reading socket failed");
        }else if(n == 0){
            printf("\nserver disconnected\n");
            done = 1;
        }
        if(done == 0){
            printf("%s", buffer);
        }
    }
    printf("exiting receive thread\n");
    pthread_exit(NULL);
    return NULL;
}

void* mainThread(void* stuff) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server, ret;
    char tempBuf[GETHOSTNAMERBUF];
    portno = PORTNUM;
    char inputBuf[SENDDATALEN];
    char username[USERNAMELEN];
    char serverName[SERVERNAMELEN];
    pthread_t serverThread;
    int serverRet = -1;
    printf("Username for Chat: ");
    fgets(username, USERNAMELEN, stdin);
    printf("IP to connect to: ");
    fgets(serverName, SERVERNAMELEN, stdin);
    username[strcspn(username, "\r\n")] = 0;
    serverName[strcspn(serverName, "\r\n")] = 0;

    if(strcmp(serverName, "localhost")==0) {
        printf("Creating Local Server Thread\n");
#ifdef LOCALSERVER
        initServer(&serverThread, portno);
        serverRet = 0;
#endif
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    printf("created socket\n");
    int hostret = gethostbyname_r(serverName, &ret, tempBuf, GETHOSTNAMERBUF, &server, NULL);
    if(hostret == -1) error("ERROR: failed to get host name");
    if(server == NULL) error("ERROR: failed to get host name");
    printf("created hostname\n");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr = *((struct in_addr *) server->h_addr_list[0]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    printf("connecting to %s\n", serverName);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR connecting");

    bzero(inputBuf, SENDDATALEN);
    read(sockfd, inputBuf, SENDDATALEN);
    printf("connected to server with other users: %s", inputBuf);
    bzero(inputBuf,SENDDATALEN);
    strncpy(inputBuf, username, USERNAMELEN);
    strncat(inputBuf, "\n", 2);
    n = write(sockfd, inputBuf, SENDDATALEN);
    if(n < 0) perror("failed to send username to server");

    int rc1, rc2;
    pthread_t sendThread, receiveThread;

    struct initStruct initDataSend;
    struct initStruct initDataRecv;
    initDataSend.username = username;
    initDataSend.sockfd = sockfd;
    initDataSend.otherThread = &sendThread;
    initDataRecv.username = username;
    initDataRecv.sockfd = sockfd;
    initDataRecv.otherThread = &receiveThread;

    rc1 = pthread_create(&sendThread, NULL, sendData, &initDataSend);
    if(rc1){
        error("ERROR: send thread failed to create.");
    }
    rc2 = pthread_create(&receiveThread, NULL, recieveData, &initDataRecv);
    if(rc2){
        error("ERROR: receive thread failed to create.");
    }

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);

    close(sockfd);

    printf("Connection Closed to %s...\n", serverName);

    if(serverRet == 0){
        printf("Destroying Local Server Thread\n");
        pthread_cancel(serverThread);
    }

    return 0;
}

int main(int argc, char *argv[]) {

    pthread_t mainThreadp;
    pthread_create(&mainThreadp, NULL, mainThread, NULL);

    pthread_join(mainThreadp, NULL);
    exit(0);
}