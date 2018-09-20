#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>


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
    char file[5000];
    int n = recvfrom(sockfd,file,5000,0,(struct sockaddr*)&clientaddr,&len);

    if (n == -1) {
      std::cout << "Timeout while waiting to recieve\n";
    }
    else {
        std::cout << "Got from client: " << file  << "\n";
	
        std::FILE*  myfile = std::fopen(file,"r");
	
      	
	if(myfile == NULL){
		std::cout  << "Error File Does Not Exsist";
		//send error to client
		continue;
	}
	std::fseek(myfile,0,SEEK_END);
	std::size_t filesize = std::ftell(myfile);
	rewind(myfile);

	long int currentsize =0;
	char line[5000];
	int packetcount = 1;
	while(currentsize < filesize){
		std::size_t read = fread(line, 1, 1024, myfile);
		sendto(sockfd,line,read,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
		currentsize += read;
		//std::cout << "Packets Sent: " << packetcount << "\n";
		std::cout << "Current Size: " << currentsize << " file size: " << filesize << " Read: " << read << "\n";
		packetcount++;
	}
      //sendto(sockfd,file,strlen(file)+1,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
    }
  }
  return 0;
}
