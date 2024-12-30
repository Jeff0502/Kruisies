#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "connect.h"
#include <sys/socket.h>
#include "consts.h"
#include "board.h"
#include <stdbool.h>
#include <unistd.h>

void runServer(void);
void runClient(void);

bool IS_GAME_RUNNING = false;
int sockfd;
char board[3][3];

int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		printf("Enter command-line arguments\n");
		exit(0);
	}

	
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			board[i][j] = '#';
		}
	}

	if(strcmp(argv[1], "server") == 0)
		runServer();

	else if(strcmp(argv[1], "client") == 0)
		runClient();

	return 0;
}

void runServer(void)
{
	printf("\nServer is up on Port: %s\n\n", PORT);
	printf("You will need to have setup port forwarding on this network. To connect externally, get your public IP address via Google and send it only to your friend.\n");
	printf("If you are both on the same local network, you can just give them your ip address from using ifconfig(LINUX) or ipconfig(Windows) in the command prompt\n\n");

	sockfd = serverDriver();
	IS_GAME_RUNNING = true;

	if(sockfd == -1)
		return;


	// Packet is in the form:	row	col	player
	//				00	00	0
	// 0 indicates a single bit
	unsigned char packet = 0x00;

	drawBoard(board);
	
	if(gameLoop(IS_GAME_RUNNING, 0, sockfd, board) == -1)
		close(sockfd);
}

void runClient(void)
{
	char ip[INET_ADDRSTRLEN];

	printf("Please enter the server's ip address:\n");

	struct sockaddr_in sa;
	int result;
	fgets(ip, sizeof(ip), stdin);

	printf("%s", ip);

	ip[strcspn(ip, "\r\n")] = 0;

	while(inet_pton(AF_INET, ip, &(sa.sin_addr)) < 0)
	{
		printf("Please enter a valid ip in the form a.b.c.d\n");
		fgets(ip, sizeof(ip), stdin);
	}

	sockfd = clientDriver(ip);


	IS_GAME_RUNNING = true;

	if(sockfd == -1)
		return;

	drawBoard(board);

	if(gameLoop(IS_GAME_RUNNING, 1, sockfd, board) == -1)
		close(sockfd);
}
