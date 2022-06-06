#include "webserver.h"

void createWebSocket(simpleWebServer *self){

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;        // IPv4 and IPv6 support
    hints.ai_flags = AI_PASSIVE;        // your local IP if AI_PASSIVE
    hints.ai_socktype = SOCK_STREAM;    // use TCP

    CHECK(getaddrinfo(NULL, self->port, &hints, &res), "getaddrinfo");
    CHECK(self->serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol), "socket");
    CHECK(bind(self->serverSocket, res->ai_addr, res->ai_addrlen), "bind");
    CHECK(listen(self->serverSocket, BACKLOG), "listen");

    freeaddrinfo(res);  
}

void run(simpleWebServer *self){

    int acceptSocket;
    struct sockaddr_storage accepted_addr;
    socklen_t sin_size;

    puts("waiting for connection...");

    while(1){
        sin_size = sizeof accepted_addr;
        if((acceptSocket = accept(self->serverSocket, (struct sockaddr *) &accepted_addr, &sin_size)) < 0){
            perror("accept");
            exit(7);
        }
        handleConnection(acceptSocket);
    }
}

void handleConnection(int acceptSocket){

    char recvBuffer[BUFFER];

    if(recv(acceptSocket, recvBuffer, BUFFER, 0) == -1){
        perror("recv");
        exit(5);
    }

    handleRequest(recvBuffer, acceptSocket);
    close(acceptSocket);
    memset(recvBuffer, 0, BUFFER);
}

void handleRequest(char* request, int socket){

    char* httpMethod;
    char* directory;
    char msg[BUFFER];
    int messageLength;

    httpMethod = strtok(request, " ");
    directory = strtok(NULL, " ");

    if(!(strcmp(directory, "/favicon.ico"))){
        return;
    }

    if((strcmp(httpMethod, "GET"))){        // only GET requests
        return;
    }

    if(!(strcmp(directory, "/"))){
        displayFile("index.html", msg);
    }else{
        directory++;
        directory = realpath(directory, NULL);
        displayFile(directory, msg);
    }
    if(errno == EFAULT){
        webError(404, socket);
        errno = 0;
    }else{
        //send message
        messageLength = strlen(msg);
        if((send(socket, msg, messageLength, 0)) < 0){
            perror("send");
            exit(6);
        }
    }
    
    memset(msg, 0, BUFFER);    
}

void displayFile(char* fileName, char msg[BUFFER]){
    FILE *f;
    char startMsg[] = "HTTP/1.1 200 OK\r\n\r\n";
    int ServerInfoLength = strlen(startMsg);
    char c;
    
    strcpy(msg, startMsg);

    f = fopen(fileName, "r");

    if(f == NULL){
        perror("file");
        return;
    }
    
    ServerInfoLength = strlen(msg);

    while((c = fgetc(f)) != EOF){
       msg[ServerInfoLength] = c;
       ServerInfoLength++;
    }
    fclose(f);
}

void webError(int errorCode, int socket){
    char *msg = "HTTP/1.1 404 Not Found\r\n\r\n<title>404 Not Found</title><h1>Not Found</h1><p>The requested URL was not found on the server.</p>";
    int messageLength;
    if(errorCode == 404){
        messageLength = strlen(msg);
        if((send(socket, msg, messageLength, 0)) < 0){
            perror("send");
            exit(6);
        }
    }
}

