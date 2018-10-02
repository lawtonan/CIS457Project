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
#include <cmath>

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
    
    if(port < 0 || port > 65535){
        std::cout << "Bad Port Number\n";
        exit(1);
    }
    
	struct timeval timeout;
    timeout.tv_sec=1;
    timeout.tv_usec=0;
    
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
    

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr= inet_addr(ipAddress);
    
    if(!inet_pton(AF_INET,ipAddress,&serveraddr.sin_addr)){
        std::cout << "Bad Address\n";
        exit(1);
    }
    
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
    
    char line2[1025];
    int len = sizeof(serveraddr);
    sendto(sockfd,line,strlen(line)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    
	int n = recvfrom(sockfd, line2, 5000, 0, (struct sockaddr*)&serveraddr,(socklen_t*)&len);
    
	if (n == -1) {
		while(n == -1){
		sendto(sockfd,line,strlen(line)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
		n = recvfrom(sockfd, line2, 5000, 0, (struct sockaddr*)&serveraddr,(socklen_t*)&len);
		
            std::cout << "Resend File\n";
		}
        }  

    int packets = 1;
    int recieve = 1024;
    char pcount[100];
    int pos;
    std::size_t fw;
    //std::size_t jank;
	//std::size_t standard;
	int jank = 1025;
	int standard = 1025;
    int nPacket = 0;
    char wait[4][1025];
    	int print;

    while(1){
        usleep(500);
        recieve = recvfrom(sockfd,line2,1025,0,(struct sockaddr*)&serveraddr,(socklen_t*)&len);

	if(packets == 1){
		jank = recieve;
		standard = recieve;
		std::cout << "RECIEVE " << recieve << " S: " << standard << " j: " << jank << "\n";
	}
	if(standard != recieve  && recieve != -1){
		jank = recieve;
		std::cout << "RECIEVE " << recieve << " S: " << standard << " j: " << jank << "\n";
	}

        pcount[0] = line2[recieve - 1];
        std::cout << "Packet Number: " << pcount[0] << "\n";
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
            // if(recieve != 1024){
            //     line2[recieve] = EOF;
            // }
            //std::cout << recieve-1 << "\n";
            
            if(atoi(pcount) == nPacket){
                fw = fwrite(line2,sizeof(char),recieve-1,myfile);
			std::cout << "\t\tPackets wrote: " << nPacket <<  "Size wrote " << recieve << "\n";
                nPacket++;
                if(nPacket == 10){
                    nPacket = 0;
                }

		print = 2;
		if(wait[0][0] != '\0'){
			print = 1;
		}
                while(print >= 1){
                    if(print == 1){
		    	if(jank != recieve && wait[1][0] == '\0' && wait[2][0] == '\0'&& wait[3][0] == '\0'){
                    fw = fwrite(wait[0],sizeof(char),jank - 1,myfile);
			std::cout << "\t\tPackets wrote: " << nPacket <<  "JANK " << jank << "\n";
	}else{
		fw = fwrite(wait[0],sizeof(char),recieve -1,myfile);
			std::cout << "\t\tPackets wrote: " << nPacket <<  "RECIEVE " << recieve << " j: " << jank << "\n";
	}
			nPacket++;
         	    }

                    for(int i = 0;i<3;i++){
                        strncpy(wait[i], wait[i+1], 1025);
			std::cout << "\t\t\twait " << i << " wait[i][0]" << wait[i][0] << " nPacket " << nPacket << " pNum " << wait[i][1024] << "\n";
                    }
                    
                    
                    
                    if(nPacket == 10){
                        nPacket = 0;
                    }

			wait[3][0] = '\0';

			if(print == 1 && wait[0][0] != '\0'){
				print = 1;
			}else if(print == 2){
				print = 0;
			}else{
				print = 2;
			}
                }
                
            }
            else {
                for (int i=1; i < 5; i++) {
                    if(atoi(pcount)-nPacket == i || atoi(pcount) - (nPacket-10)==i){
                        strncpy(wait[i-1], line2, 1025);
			wait[i-1][1024] = line2[recieve - 1]; 
			std::cout << "WROTE pcount " << pcount << " While nPacket " << nPacket << " To wait[i] i= " << i << "\n";
                    }
                }
            }
	std::cout << "\t SENDING BACK: " << pcount << "\n";
        	sendto(sockfd,pcount,strlen(pcount)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

	packets++;
        }
        
        
        //std::cout << "Packets recieved: " << packets << "\n";
        
        // want to send the number of the packet recieved back to the server
        //		pcount = string(itoa(packets));
        // send a message to server saying the the packet was recieved
        //sendto(sockfd,pcount,strlen(pcount)+1,0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
        
    }
    
    fclose(myfile);
    
    close(sockfd);
    
    return 0;
}
