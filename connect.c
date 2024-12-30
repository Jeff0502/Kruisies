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

void *convertInAddr(struct sockaddr *sa)
{
	// IPv4
	if(sa->sa_family == AF_INET)
		return &((struct sockaddr_in*)sa)->sin_addr;
	else 
		return &((struct sockaddr_in6*)sa)->sin6_addr;
}

int setInfo(char* ip, struct addrinfo *hints, struct addrinfo **servInfo, int* sockfd)
{
	int status;

	// IPv4 or v6
	hints->ai_family = AF_INET6;
	hints->ai_socktype = SOCK_STREAM;

	if((status = getaddrinfo(ip, PORT, hints, servInfo)) != 0)
	{
		fprintf(stderr, "Error from getaddrinfo: %s\n", gai_strerror(status));
		
		return status;
	}

	return 0;
}

// Return the socket file descriptor
int serverDriver(void)
{
	struct sockaddr_storage connAddr;
	socklen_t sin_size;
	int status, sockfd, clientFd;
	struct addrinfo hints, *servinfo, *p;
	int yes = 1;

	memset(&hints, 0, sizeof(hints));

	hints.ai_flags = AI_PASSIVE;

	if(setInfo(NULL, &hints, &servinfo, &sockfd) != 0)
		return -1;

	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			// Server
			perror("Error on socket(), server");
			continue;
		}

		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt() error:");
			perror(NULL);

			return -1;
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("Failed to bind socket\n");
			perror(NULL);

			continue;
		}

		break;
	}

	if(p == NULL)
	{
		perror("Failed to bind to a socket");
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

	return clientFd;
}

int clientDriver(char* ip)
{
	int status, sockfd;
	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));

	if(setInfo(ip, &hints, &servinfo, &sockfd) != 0)
		return -1;


	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			// Client
			perror("Error on socket(), client");
			continue;
		}

		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("Failed to connect");
			perror(NULL);
			continue;
		}

		break;
	}

	if(p == NULL)
	{
		perror("Failed to connect to a socket");
		return -1;
	}


	freeaddrinfo(servinfo);
	printf("Connected to server. Waiting for server to initialize.\n");

	return sockfd;
}

