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

	return 0;
}

// Return the socket file descriptor
int serverDriver(void)
{
	struct sockaddr_storage connAddr;
	socklen_t sin_size;
	int status, sockfd, clientFd;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));

	// IPv4
	hints.ai_flags = AI_PASSIVE;

	if(setInfo(NULL, &hints, &servinfo, &sockfd) != 0)
		return -1;

	int yes = 1;

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt() error:");
		perror(NULL);

		return -1;
	}

	if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		perror("Failed to bind socket\n");
		perror(NULL);

		return -1;
	}

	freeaddrinfo(servinfo);

	// Listen etc.
	if((listen(sockfd, 5)) == -1)
	{
		perror("Failed on listen()\n");
		perror(NULL);

		return -1;
	}


	sin_size = sizeof(connAddr);

	printf("Waiting for client to connect...\n");

	if((clientFd = accept(sockfd, (struct sockaddr*)&connAddr, &sin_size)) == -1)
	{
		perror("Failed on accept()\n");
		perror(NULL);

		return -1;
	}

	printf("Client connected. Initializing game.\n");

	// Maybe close?
	// char *buf = "HELLO WORLD!";


	return clientFd;
}

int clientDriver(const char* ip)
{
	int status, sockfd;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));

	// IPv4
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if(setInfo("127.0.0.1", &hints, &servinfo, &sockfd) != 0)
		return -1;

	if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
	{
		close(sockfd);
		perror("Failed to connect");
		perror(NULL);

		return -1;
	}

	freeaddrinfo(servinfo);
	printf("Connected to server. Waiting for server to initialize.\n");

	// Maybe close?

	return sockfd;
}

