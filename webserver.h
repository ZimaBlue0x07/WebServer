#ifndef SIMPLEWEBSERVER_H_
#define SIMPLEWEBSERVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <limits.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <pthread.h>

#define BACKLOG 10
#define BUFFER 4096

#define CHECK(x,m) do{ if((x) < 0) { perror(m); exit(1); } } while(0)

typedef struct simplewebserver simpleWebServer;

struct simplewebserver
{
    int serverSocket;
    const char* port;                        
    void (*run)(simpleWebServer *self);
    void (*createWebSocket)(simpleWebServer *self);
};

void webError(int errorCode, int socket);
void displayFile(char* fileName, char msg[BUFFER]);
void handleRequest(char* request, int socket);
void handleConnection(int acceptSocket);
void createWebSocket(simpleWebServer *self);
void run(simpleWebServer *self);

#endif
