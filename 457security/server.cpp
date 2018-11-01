#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <pthread.h>

bool running = true;

void* handleclient(void* arg) {
    while (running) {
        int clientsocket = *(int*)arg;
        char line[5000];
        //char line2[5000];
        recv(clientsocket, line, 5000, 0);
        if(running) {
            std::cout << "Got from client: " << line << "\n";
        }
        //send(clientsocket, line, strlen(line)+1, 0);
        if(strcmp(line, "Quit") == 0) {
            std::cout << "Exiting Server\n";
            running = false;
            exit(1);
        }
    }
    return 0;
}

int main(int arc, char** argv) {
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    
    if (sockfd<0) {
        std::cout << "Problem creating socket\n";
        return 1;
    }
    
    int port;
    std::cout << "Input a port number: ";
    std::cin >> port;
    
    struct sockaddr_in serveraddr, clientaddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(port);
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    
    int b = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    
    if(b<0) {
        std::cout << "Bind error\n";
        return 3;
    }
    listen(sockfd,10);
    
    int first = 1;
    
    while(running){
        int len = sizeof(clientaddr);
        std::cout << "Got here\n";
        int clientsocket;
        if (first == 1) {
            clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr, (socklen_t*)&len);
        }
        pthread_t child;
        pthread_create(&child,NULL,handleclient,&clientsocket);
        pthread_detach(child);
        
        char line[5000];
        std::cout << "Enter a Message: ";
        if (first == 1) {
            std::cin.ignore();
            first--;
        }
        std::cin.getline(line,5000);
        
        if(!running) {
            break;
        }
        
        send(clientsocket, line, strlen(line)+1, 0);
        
        if(strcmp(line, "Quit") == 0) {
            std::cout << "Exiting Server\n";
            return 1;
        }
        
    }
    
    
    return 0;
}
