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

	// current spot that were at when reading / transfering the file
	long int currentsize =0;
	char line[5000];
	//total packets sent
	int packetcount = 1;
	//Packets currently in transition
	int sendingPackets = 0;
	//recieve is set as -1 as recv returns -1 on error so i start off by setting it as error as nothing was read in
	int recieve = -1;
	//intended for this to be the packet number that the client confirms that it recieved
	char pcount[100];

	//while there are still packets out, or theres more of the file to go continue
	while(currentsize < filesize || sendingPackets > 0){
		//if theres less than 5 packets out send a packet
		//i added currentsize < filesize to make sure we dont try to send an extra packet might be redundant
		while(currentsize < filesize && sendingPackets < 5){
		
			std::size_t read = fread(line, 1, 1024, myfile);
			sendto(sockfd,line,read,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
			currentsize += read;

			//std::cout << "Packets Sent: " << packetcount << "\n";
			std::cout << "Current Size: " << currentsize << " file size: " << filesize << " Read: " << read << " Packets out: " << sendingPackets << "\n";
			packetcount++;
			sendingPackets++;
		}
		

		recieve = recvfrom(sockfd,pcount,100,0,(struct sockaddr*)&clientaddr,&len);
		if(recieve != -1){
			std::cout << "Packets recieved by client " << pcount << " Packets out: " << sendingPackets << "\n";
			//packs out goes down by one 
			sendingPackets--;
			recieve = -1;
		}

	}
      //sendto(sockfd,file,strlen(file)+1,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
    }
  }
  return 0;
}
