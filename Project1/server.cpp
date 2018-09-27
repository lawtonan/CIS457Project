#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>



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
    
	if(port < 0 || port > 65535){
		std::cout << "Bad Port Number\n";
		return 1;
	}

    struct sockaddr_in serveraddr, clientaddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(port);
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    
    int b = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    
    if (b < 0) {
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
                std::cout  << "Error File Does Not Exsist\n";
                //send error to client
                char fail[1024] = "Error";
                sendto(sockfd,fail,1024,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
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
            int extra;
            std::size_t read;
            char header[10] = {'0','1','2','3','4','5','6','7','8','9'};
            int pNum = 0;
            std::vector<char> array;

	    char keep1[1025];
	    char keep2[1025];
	    char keep3[1025];
	    char keep4[1025];
	    char keep5[1025];

            //while there are still packets out, or theres more of the file to go continue
            while(currentsize < filesize || sendingPackets > 0){
                //if theres less than 5 packets out send a packet
                //i added currentsize < filesize to make sure we dont try to send an extra packet might be redundant
                while(currentsize < filesize && sendingPackets < 5){
                    
                    if(filesize - 1024 >= currentsize){
                        read = fread(line, 1, 1024, myfile);
                        array.push_back(header[pNum]);
                    }else{
                        extra = filesize - currentsize;
                        read = fread(line, 1, extra ,myfile);
                        array.push_back(header[pNum]);
                    }
                    line[read] = header[pNum];
                    sendto(sockfd,line,read+1,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
                    
			sendingPackets++;
			if(sendingPackets == 1){
				for(int i = 0;i<read;i++){		
					keep1[i] = line[i];
				}
			}else if(sendingPackets == 2){
				for(int i = 0;i<read;i++){		
					keep2[i] = line[i];
				}
			}else if(sendingPackets == 3){
				for(int i = 0;i<read;i++){		
					keep3[i] = line[i];
				}
			}else if(sendingPackets == 4){
				for(int i = 0;i<read;i++){		
					keep4[i] = line[i];
				}
			}else if(sendingPackets == 5){
				for(int i = 0;i<read;i++){		
					keep5[i] = line[i];
				}			
			}

                    pNum++;
                    if (pNum == 10) {
                        pNum = 0;
                    }

                    currentsize += read;
                    
                    //std::cout << "Packets Sent: " << packetcount << "\n";
                    std::cout << "Total packets out: " << packetcount << "pNum=" << pNum << "\n";
                    packetcount++;
                    
                }
                
                
                recieve = recvfrom(sockfd,pcount,100,0,(struct sockaddr*)&clientaddr,&len);
                for (int i = 0; i < sendingPackets; i++) {
                    if (array[i] == pcount[0]) { //first bit
                        array[i] = ' ';
                    }
                }
		if(recieve == -1){
			std::cout << "Packet lost Resending packet\n" ;
			 sendto(sockfd,keep1,read+1,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
		}else if(array[0] == ' ') {
		    while(array[0] == ' '){
                    //std::cout << "Packet recieved by client:" << pcount[0] << " Packets currently out: " << sendingPackets << "\n";
                    //packs out goes down by one
                    	for (int j = 0; j < sendingPackets; j++) {
                    	    array[j] = array[j+1];
                    	}
                    	std::cout << array[0] << array[1] << array[2] << array[3]<< array[4] << "\n";
                    	array.resize(array.size()-1); 

		    	for(int i = 0;i<1025;i++){
				keep1[i] = keep2[i];
				keep2[i] = keep3[i];
				keep3[i] = keep4[i];
				keep4[i] = keep5[i];
      		    	}
		    	
 
                    	sendingPackets--;
                    	recieve = -1;
	            }
                }
                
            }
            char end[1024] = "EOF";
            sendto(sockfd,end,strlen(end)+1,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
        }
    }
    return 0;
}
