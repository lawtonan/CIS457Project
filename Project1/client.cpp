#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

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
    char fout[5000];
    //std::string input;
    std::cout << "Enter a file to copy: ";
    std::cin.ignore();
    std::cin.getline(line,5000);
    std::cout << "Enter a file to write to: ";
    std::cin.getline(fout,5000);

    
    std::FILE* myfile = std::fopen(fout,"w");
    if(myfile == NULL){
        return 1;
    }
    
    char line2[1024];
    int len = sizeof(serveraddr);
    sendto(sockfd,line,strlen(line)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    // recvfrom(sockfd, line2, 5000, 0, (struct sockaddr*)&serveraddr,(socklen_t*)&len);
    
    int packets = 1;
    int recieve = 1024;
    char pcount[100];
    int pos;
    std::size_t fw;

    while(1){
        
        recieve = recvfrom(sockfd,line2,1024,0,(struct sockaddr*)&serveraddr,(socklen_t*)&len);
        if (strcmp(line2,"EOF") == 0) {
            std::cout << "End of file reached.\n";
            break;
        }
        if (strcmp(line2,"Error") == 0) {
            std::cout << "File does not exist.\n";
            break;
        }
        if(recieve != -1){
            //	pos = strlen(line2) - 1;
            if(recieve != 1024){
                line2[recieve-1] = '\n';
                line2[recieve] = '\0';
            }
            fw = fwrite(line2,sizeof(char),recieve,myfile);
        }			
        
        std::cout << "Packets recieved: " << packets << "\n";
        packets++;
        // want to send the number of the packet recieved back to the server
        //		pcount = string(itoa(packets));
        // send a message to server saying the the packet was recieved
        sendto(sockfd,pcount,strlen(pcount)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));	
        
    }
    
    fclose(myfile);
    
    close(sockfd);
    
    return 0;
}
