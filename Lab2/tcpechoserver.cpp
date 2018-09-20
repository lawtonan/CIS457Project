#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>


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
  while(1){
    int len = sizeof(clientaddr);
    int clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr, (socklen_t*)&len);
    char line[5000];
    char line2[5000];
    recv(clientsocket, line, 5000, 0);
    std::cout << "Got from client: " << line << "\n";

    std::ifstream myfile;
    myfile.open(line);
    if(myfile.fail()) {
      std::cerr << "Error: " << strerror(errno) << "\n";
      send(clientsocket, line2, strlen(line2)+1, 0);
      continue;
    }

    std::string word;
    std::string words;
    while(std::getline(myfile,word)) {
      words = words + word;
    }
    strcpy(line2,words.c_str());

    send(clientsocket, line2, strlen(line2)+1, 0);

    std::cout << "Data from " << line << " sent to Client.\n";

    close(clientsocket);
  }


	return 0;
}
