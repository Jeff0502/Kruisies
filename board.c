#include "board.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include "consts.h"
#include "packet.h"

void exitHost(int sockfd);

void handleErr(int boardErr)
{
	switch(boardErr)
	{
		case BOARD_BOUND_ERR:
			printf("Please enter a number between 0 and 2\n");
		break;
		case ALREADY_FILLED:
			printf("The position is already filled. Enter a new position.\n");
		break;
	}
}

void drawBoard(char board[3][3])
{
	fprintf(stdout, "===================================");
	for(int i = 0; i < 3; i++)
	{
		fprintf(stdout, "\n");
		for(int j = 0; j < 3; j++)
		{
			fprintf(stdout, " %c ", (char)board[i][j]);
		}
		
		fprintf(stdout, "\n");
	}
	
	fprintf(stdout, "===================================");
	fprintf(stdout, "\n");
}

int placeMarker(int row, int col, int player, char board[3][3])
{
	if(row < 0 || row > 2 || col < 0 || col > 2)
		return BOARD_BOUND_ERR;

	if(player < 0 || player > 1)
		return PLAYER_ERR;

	if(board[row][col] != '#')
		return ALREADY_FILLED;

	char marker;

	if(player == 0)
		marker = 'X';
	else if (player == 1)
		marker = 'O';

	board[row][col] = marker;
	drawBoard(board);

	return 0;
}


int gameLoop(bool IS_GAME_RUNNING, bool player, int sockfd, char board[3][3])
{
	bool playerTurn = 0;
	unsigned char row, col, code, buf;
	int read_result, boardErr = 0;

	while(IS_GAME_RUNNING)
	{
		if(playerTurn == player)
		{
			do{
				handleErr(boardErr);
				printf("Enter the row and column [r, c]:\n ");
				if(scanf(" [%hhu, %hhu]", &row, &col) == EOF)
				{
					printf("Control sequence received. Exiting...\n");
					exitHost(sockfd);
					return 0;
				}

			}while((boardErr = placeMarker(row, col, player, board)) != 0);


			code = encode(row, col, player);

			if(send(sockfd, (unsigned char*)&code ,sizeof(unsigned char), 0) == -1)
			{
				perror("Error in send()");
				return -1;
			}


			playerTurn = !playerTurn;
		}

		else 
		{
			printf("Waiting for player %d...\n ", !player);

			if((read_result = recv(sockfd, (unsigned char*)&buf, sizeof(unsigned char), 0)) == -1)
			{
				perror("Client receive");
				perror(NULL);
				return -1;
			}

			if(buf == 0xFF)
			{
				printf("Player %d has disconnected\n", !player);
				printf("Exiting...\n");
				return -1;
			}

			struct packet p = decode(buf);
			
			placeMarker(p.row, p.col, !player, board);
			playerTurn = !playerTurn;
		}
	}

	return 0;
}

void exitHost(int sockfd)
{
	char exit = 0xFF;
	if(send(sockfd, (unsigned char*)&exit, sizeof(unsigned char), 0) == -1)
	{
		perror("Could not confirm exit.\n");
	}

	close(sockfd);
}
