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
	if (sockfd < 0) {
		std::cout << "There was an error creating the socket\n";
		return 1;
	}

	char ipAddress[5000];
	int port;
	std::cout << "Enter an IP address: ";
	std::cin >> ipAddress;
	std::cout << "Enter a Port number: ";
	std::cin >> port;

	struct sockaddr_in serveraddr;
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(port);
	serveraddr.sin_addr.s_addr=inet_addr(ipAddress);

	int e = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	if (e<0){
		std::cout << "There was an error connecting\n";
		return 2;
	}

	std::cout << "Conected to server.\n";

	char line[5000];
	char line2[5000];
	std::cout << "Enter a File name: ";
	std::cin >> line;

	send(sockfd, line, strlen(line)+1, 0);
	recv(sockfd, line2, 5000, 0);

	if (strlen(line2) == 0) {
		std::cerr << "File did not exist or was empty" << "\n";
		return 3;
	}

	std::ofstream newfile;
	newfile.open("NewFile.txt");
	newfile << line2;
	newfile.close();

	std::cout << "File was created: NewFile.txt\n";

	return 0;
}
