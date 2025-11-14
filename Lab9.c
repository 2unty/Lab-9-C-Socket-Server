#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
const char* PORT = "1818";
struct addrinfo hints, *first, *current;
int mainSocket=-1;
int newSocket;
int main(int argc, char *argv[]){
	if(argc==1){
		struct sockaddr otherAddr;
		int backLog;
		socklen_t size;
		memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		int ret = getaddrinfo(NULL,PORT,&hints,&first);
		//printf("Finding socket\n");
		//printf("getaddrinfo: %s\n",gai_strerror(ret));
		for(current = first; current != NULL; current = current->ai_next){
			if((mainSocket = socket(first->ai_family,first->ai_socktype,first->ai_protocol))==-1){
				continue;
			}
			if(bind(mainSocket, current->ai_addr, current->ai_addrlen)==-1){
		        close(mainSocket);
		        mainSocket = -1;
				continue;
			}
			//otherAddr = current->ai_addr;
			break;
		}
		freeaddrinfo(first);
		printf("Listening...\n");
		if(listen(mainSocket,backLog)==-1){
		        close(mainSocket);
		        return 1;
		        //continue;
		}
		//printf("Main: %d",mainSocket);
		if((newSocket = accept(mainSocket,(struct sockaddr *)&otherAddr,&size))==-1){
			perror("Couldn't accept!");
		}
		printf("Connected!\n");
		size = sizeof(otherAddr);
		char readBuffer[1000];
		char inputBuffer[1000];
		int listening = 1;
		int readbytes;
		while(1){
			if(newSocket<0){
	            printf("Closing server.\n");
	            return 0;
	        }
			if(listening==1){
			    printf("Reading from client...\n");
			    if((readbytes = recv(newSocket,readBuffer,1000,0))>0){
			        printf("ping: %s",readBuffer);
			        sprintf(readBuffer," ");
			        listening = 0;
			    }
			}else{
			    printf("Write to client: ");
			    fgets(inputBuffer,1000,stdin);
			    send(newSocket,inputBuffer,1000,0);
			    sprintf(inputBuffer," ");
			    listening = 1;
	        }
		}
	}
	else if(argc>1&&argc<4){
        const char *serverIP = argv[1];
        const char *port = argv[2];
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int ret = getaddrinfo(serverIP,port,&hints,&first);
        printf("Connecting to IP: %s, PORT: %s\n",serverIP,port);
        for(current = first; current!=NULL; current = current->ai_next){ // What specifically does this linked list iterate through?
            if((mainSocket=socket(first->ai_family,first->ai_socktype,first->ai_protocol))<0){
            	continue;
            }
            if(connect(mainSocket,current->ai_addr,current->ai_addrlen)!=0){
                close(mainSocket);
                mainSocket = -1;
                continue;
            }
            break;
            //printf("Connect: %d\n",conn);
                
        }
        freeaddrinfo(first);
        if(mainSocket<0){
            printf("Couldn't connect!\n");
            return 1;
        }
        printf("Connected!\n");
        char readBuffer[1000];
		char inputBuffer[1000];
		int listening = 0;
		while(1){
		    if(mainSocket<0){
		            printf("Closing client\n");
		            return 0;
		    }
		    if(listening==1){
		        printf("Reading from server...\n");
		        if(recv(mainSocket,readBuffer,1000,0)>0){
		            printf("pong: %s",readBuffer);
		            sprintf(readBuffer," ");
		            listening = 0;
		        }
		    }else{
		        printf("Write to server: ");
		        fgets(inputBuffer,1000,stdin);
		        send(mainSocket,inputBuffer,1000,0);
		        sprintf(inputBuffer," ");
		        listening = 1;
			}
        }
    }
}
