#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "connect.h"
#include <sys/socket.h>
#include "consts.h"
#include "board.h"

int main(int argc, char* argv[])
{
	int sockfd;
	if(argc == 1)
	{
		printf("Enter command-line arguments\n");
		exit(0);
	}

	// char board[3][3];
	//
	// for(int i = 0; i < 3; i++)
	// {
	// 	for(int j = 0; j < 3; j++)
	// 	{
	// 		board[i][j] = '#';
	// 	}
	// }
	//
	// drawBoard(board);
	//
	// placeMarker(0, 1, 1, board);
	// drawBoard(board);
	

	if(strcmp(argv[1], "server") == 0)
	{
		sockfd = serverDriver();

		if(send(sockfd, "Hello World", 13, 0) == -1)
		{
			perror("Error in send()");
		}
	}

	else if(strcmp(argv[1], "client") == 0)
	{
		sockfd = clientDriver("127.0.0.1");
		char buf[80];

		int read_result;

		if((read_result = recv(sockfd, buf, 80, 0)) == -1)
		{
			perror("Client receive");
			perror(NULL);
			return -1;
		}

		printf("%s", buf);
	}

	return 0;
}
