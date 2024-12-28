#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "connect.h"
#include <sys/socket.h>
#include "consts.h"
#include "board.h"
#include <stdbool.h>

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
	sockfd = serverDriver();
	IS_GAME_RUNNING = true;

	// Packet is in the form:	row	col	player
	//				00	00	0
	// 0 indicates a single bit
	unsigned char packet = 0x00;

	drawBoard(board);
	
	gameLoop(IS_GAME_RUNNING, 0, sockfd, board);
}

void runClient(void)
{
	sockfd = clientDriver("127.0.0.1");
	IS_GAME_RUNNING = true;

	drawBoard(board);

	gameLoop(IS_GAME_RUNNING, 1, sockfd, board);
}
