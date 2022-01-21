// Example posted on ece312 Moodle site
// gcc tcpserver.c -o server -pthread
// ./server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 4567
#define BUF_SIZE 255
#define CLADDR_LEN 100
#define MAX_CONNECTIONS 5
#define USERNAMELEN 30

void error(char *msg) {
    sprintf(msg, "SERVER: %s", msg);
    perror(msg);
    exit(1);
}

void servPrint(char* msg){
    printf("SERVER: %s", msg);
}

pthread_mutex_t connMutex = PTHREAD_MUTEX_INITIALIZER;
int connList[MAX_CONNECTIONS-1];
char usernames[MAX_CONNECTIONS][USERNAMELEN];

void* receiveMessage(void* number) {
    int sockfd, ret, n, numCon;
    int* temp = (int*) number;
    char buffer[BUF_SIZE];
    numCon = *temp;
    pthread_mutex_lock(&connMutex);
    sockfd = connList[numCon];
    pthread_mutex_unlock(&connMutex);

    memset(buffer, 0, BUF_SIZE);

    pthread_mutex_lock(&connMutex);
    snprintf(buffer, BUF_SIZE, " ");
    int i = 0;
    while(i < MAX_CONNECTIONS){
        if(i != numCon && connList[i] != 0){
            strncat(buffer, usernames[i], USERNAMELEN);
            strncat(buffer, " ", 2);
        }
        i++;
    }
    strncat(buffer, "\n", 2);
    n = send(sockfd, buffer, BUF_SIZE, 0);
    if(n < 0){
        servPrint("ERROR: failed to send usernames to client");
    }
    bzero(buffer, BUF_SIZE);
    n = recv(sockfd, buffer, BUF_SIZE,0);
    if(n < 0){
        servPrint("ERROR: failed to get client username");
    }
    strncpy(usernames[numCon], buffer, USERNAMELEN);
    i = 0;
    while(i < MAX_CONNECTIONS){
        bzero(buffer, BUF_SIZE);
        snprintf(buffer, BUF_SIZE, "%s joined the chat!\n", usernames[numCon]);
        if(i != numCon && connList[i] != 0) n = send(connList[i], buffer, BUF_SIZE, 0);
        if(n < 0){
            servPrint("ERROR: failed to send data to a client");
        }
        i++;
    }
    pthread_mutex_unlock(&connMutex);

    while ((ret = recv(sockfd, buffer, BUF_SIZE,0)) > 0) {
        printf("SERVER: CLIENT: %s", buffer);
        buffer[strcspn(buffer, "\r\n")] = 0;
        if(strcmp(buffer, "quit") == 0){
            servPrint("Closing connection\n");
            close(sockfd);
            pthread_mutex_lock(&connMutex);
            i = 0;
            while(i < MAX_CONNECTIONS){
                bzero(buffer, BUF_SIZE);
                snprintf(buffer, BUF_SIZE, "%s left the chat!\n", usernames[numCon]);
                if(i != numCon && connList[i] != 0) n = send(connList[i], buffer, BUF_SIZE,0);
                if(n < 0){
                    servPrint("ERROR: failed to send data to a client");
                }
                i++;
            }
            pthread_mutex_unlock(&connMutex);
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&connMutex);
        i = 0;
        while(i < MAX_CONNECTIONS){
            if(i != numCon && connList[i] != 0) n = send(connList[i], buffer, BUF_SIZE, 0);
            if(n < 0){
                servPrint("ERROR: failed to send data to a client");
            }
            i++;
        }
        pthread_mutex_unlock(&connMutex);
        printf("sent Last message\n");
    }
    if (ret < 0)
        servPrint("Error receiving data!\n");
    else
        servPrint("Closing connection\n");
    close(sockfd);
}

void* mainThread(void* argvin){
    char** argv = (char **) argvin;
    int sockfd, newsockfd, portno, clilen;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    pid_t childpid;
    char clientAddr[CLADDR_LEN];
    pthread_t rThread[MAX_CONNECTIONS-1];
    int ret, n;
    portno = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    servPrint("Socket created...\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    servPrint("Binding done...\n");

    printf("Waiting for a connection...\n");
    listen(sockfd, MAX_CONNECTIONS);
    clilen = sizeof(cli_addr);

    ;
    while (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))
    {
        inet_ntop(AF_INET, &(cli_addr.sin_addr), clientAddr, CLADDR_LEN);
        printf("SERVER: Connection accepted from %s...\n", clientAddr);

        printf("still good here\n");
        pthread_mutex_lock(&connMutex);
        printf("made it here1\n");
        int connNum = MAX_CONNECTIONS-1;
        int i = 0;
        while(i < MAX_CONNECTIONS){
            printf("cycling list on i: %d\n", i);
            if(connList[i] == 0){
                connList[i] = newsockfd;
                connNum = i;
                break;
            }
            i++;
        }
        printf("%d\n", connNum);
        i = 0;
        while(i < MAX_CONNECTIONS){
            bzero(buffer, BUF_SIZE);
            snprintf(buffer, BUF_SIZE, "accepting connection from %s\n", clientAddr);
            if(i != connNum && connList[i] != 0) n = send(connList[i], buffer, BUF_SIZE,0);
            if(n < 0){
                servPrint("ERROR: failed to send data to a client");
            }
            i++;
        }
        pthread_mutex_unlock(&connMutex);

        //creating a new thread for receiving messages from the client
        ret = pthread_create(&(rThread[connNum]), NULL, receiveMessage, &connNum);
        if (ret) {
            printf("SERVER: ERROR: Return Code from pthread_create() is %d\n", ret);
            error("ERROR creating thread");
        }
    }

    if (newsockfd < 0)
        error("ERROR on accept");
//        close(newsockfd);
    close(sockfd);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "SERVER: ERROR: no port provided\n");
        exit(1);
    }

    pthread_t mainThreadp;
    pthread_create(&mainThreadp, NULL, mainThread, (void *) argv);

    pthread_join(mainThreadp, NULL);
    exit(0);
}
