#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg) {
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]) {
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256] = "This is a string from client!";

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <server adddress> <server port>\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);

	printf("\nIPv4 TCP Client Started...\n");
	
	//Sockets Layer Call: socket()
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	//Sockets Layer Call: gethostbyname()
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	memmove((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	//Sockets Layer Call: connect()
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");

	
	//Sockets Layer Call: send()
	n = send(sockfd,buffer, strlen(buffer)+1, 0);
	if (n < 0)
		error("ERROR writing to socket");

	memset(buffer, 0, 256);
	
	//Sockets Layer Call: recv()
	n = recv(sockfd, buffer, 255, 0);
	if (n < 0)
		error("ERROR reading from socket");
	printf("Message from server: %s\n", buffer);

	//Sockets Layer Call: close()
	close(sockfd);
		
	return 0;
}
