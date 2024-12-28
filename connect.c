#include "connect.h"
#include <asm-generic/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT "8888"

// Some code from https://beej.us/guide/bgnet/html/split/system-calls-or-bust.html#bind

int setInfo(char* ip, struct addrinfo *hints, struct addrinfo **servInfo, int* sockfd)
{
	int status;
	if((status = getaddrinfo(ip, PORT, hints, servInfo)) != 0)
	{
		fprintf(stderr, "Error from getaddrinfo: %s\n", gai_strerror(status));
		
		return status;
	}

	// IPv4 or v6
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_STREAM;

	if((*sockfd = socket((*servInfo)->ai_family, (*servInfo)->ai_socktype, (*servInfo)->ai_protocol)) == -1)
	{
		// Server
		if(ip == NULL)
			perror("Error on socket(), server");

		else
			perror("Error on socket(), server");
	}
}

int serverDriver(void)
{
	struct sockaddr_storage connAddr;
	socklen_t sin_size;
	int status, sockfd, clientFd;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof(hints));

	// IPv4
	hints.ai_flags = AI_PASSIVE;

	setInfo(NULL, &hints, &servinfo, &sockfd);

	int yes = 1;

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt");
		perror(NULL);

		return -1;
	}

	if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		fprintf(stderr, "Failed to bind socket\n");
		perror(NULL);

		return -1;
	}

	freeaddrinfo(servinfo);

	// Listen etc.
	if((listen(sockfd, 5)) == -1)
	{
		fprintf(stderr, "Failed on listen()\n");
		perror(NULL);

		return -1;
	}


	sin_size = sizeof(connAddr);

	printf("Accepting\n");

	if((clientFd = accept(sockfd, (struct sockaddr*)&connAddr, &sin_size)) == -1)
	{
		fprintf(stderr, "Failed on accept()\n");
		perror(NULL);

		return -1;
	}

	// Maybe close?
	// char *buf = "HELLO WORLD!";

	if(send(clientFd, "Hello World", 13, 0) == -1)
	{
		perror("send");
	}

	return 0;
}

int clientDriver(const char* ip)
{
	int status, sockfd;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));

	// IPv4
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	setInfo("127.0.0.1", &hints, &servinfo, &sockfd);

	if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
	{
		close(sockfd);
		perror("client: connect");
		return -1;
	}
	freeaddrinfo(servinfo);

	// Maybe close?
	char buf[80];

	int read_result;

	if((read_result = recv(sockfd, buf, 80, 0)) == -1)
	{
		perror("recv");
		return -1;
	}

	// buf[79] = '\n';

	printf("%s", buf);

	// Clean up

	return 0;
}

