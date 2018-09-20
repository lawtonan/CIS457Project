#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    std::cout << "There was a problem creating the socket\n";
    return 1;
  }

  char ipAddress[5000];
	int port;
	std::cout << "Enter an IP address: ";
	std::cin >> ipAddress;
	std::cout << "Enter a Port number: ";
	std::cin >> port;

  struct sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(port);
  serveraddr.sin_addr.s_addr= inet_addr(ipAddress);

  char line[5000];
  //std::string input;
  std::cout << "Enter a message: ";
  std::cin.ignore();
  std::cin.getline(line,5000);
  //strcpy(line, input.c_str());


  char line2[5000];
  int len = sizeof(serveraddr);
  sendto(sockfd,line,strlen(line)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
  recvfrom(sockfd, line2, 5000, 0, (struct sockaddr*)&serveraddr,(socklen_t*)&len);

  std::cout << "Server returned: " << line2 << "\n";

  close(sockfd);

  return 0;
}
