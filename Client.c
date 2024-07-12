#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //defines the hostent struct 

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char const *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[255];
	if (argc < 3){
		error("Error 001:\nMissing args Error;\nFormat:\nargv[1] = server.ip.addr\nargv[2] = port\n");
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		error("Error 002: Failed to open socket.");
	}
	server = gethostbyname(argv[1]);
	if (server == NULL){
		error("Error 003: No such host");
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server ->h_addr, (char *) &serv_addr.sin_addr.s_addr, server ->h_length);
	serv_addr.sin_port =  htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("Error 004: Connection Failed");
	}
	while(1){
		bzero(buffer, 255);
		fgets(buffer, 255, stdin);
		n = write(sockfd, buffer, strlen(buffer));
		if(n < 0){
			error("Error 005: Writing Failed");
		}
		bzero(buffer, 255);
		n = read(sockfd, buffer, 255);
		if(n < 0){
			error("Error 006: Reading Failed");
		}
		printf("[+] Server %s\n", buffer);

		int i = strncmp("Close Connection", buffer, 16);
        if (i == 0){
            break;
        }
    }
    close(sockfd);
	return 0;
}