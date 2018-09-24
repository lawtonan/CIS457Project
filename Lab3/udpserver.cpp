#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

int main (int argc, char** argv) {
  int sockfd = socket(AF_INET,SOCK_DGRAM,0);

  if (sockfd<0) {
    std::cout << "Problem creating socket\n";
    return 1;
  }

  struct timeval timeout;
  timeout.tv_sec=5;
  timeout.tv_usec=0;

  setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

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

  while(1) {
    socklen_t len = sizeof(clientaddr);
    char line[5000];
    int n = recvfrom(sockfd,line,5000,0,(struct sockaddr*)&clientaddr,&len);

    if (n == -1) {
      std::cout << "Timeout while waiting to recieve\n";
    }
    else {
      std::cout << "Got from client: " << line << "\n";
      sendto(sockfd,line,strlen(line)+1,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
    }
  }
}
