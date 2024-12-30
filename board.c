#include "board.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include "consts.h"
#include "packet.h"

int moveCount = 0;

void exitHost(int sockfd, const char *msg, char exit);

void handleErr(int boardErr)
{
	switch(boardErr)
	{
		case BOARD_BOUND_ERR:
			printf("Please enter a number equal to or between 1 and 3\n");
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

			if((char)board[i][j] == 'X')
				fprintf(stdout, "\033[91m %c \033[39m", (char)board[i][j]);
			else if((char)board[i][j] == 'O')
				fprintf(stdout, "\033[92m %c \033[39m", (char)board[i][j]);
			else 
				fprintf(stdout, " %c ", (char)board[i][j]);
		}
		
		fprintf(stdout, "\n");
	}
	
	fprintf(stdout, "===================================");
	fprintf(stdout, "\n");
}

int placeMarker(int row, int col, int player, char board[3][3])
{
	if(row < 1 || row > 3 || col < 1 || col > 3)
		return BOARD_BOUND_ERR;

	if(player < 0 || player > 1)
		return PLAYER_ERR;

	row--;
	col--;

	if(board[row][col] != '#')
		return ALREADY_FILLED;

	char marker;

	marker = (player == 0) ? 'X' : 'O';

	board[row][col] = marker;
	drawBoard(board);
	moveCount++;
	
	// From https://stackoverflow.com/questions/1056316/algorithm-for-determining-tic-tac-toe-game-over
	// Check column wins:
	for(int i = 0; i < 3; i++)
	{
		if(board[row][i] != marker)
			break;
		if(i == 2)
			return (player == 0) ? HOST_WON : CLIENT_WON;
	}

	// Check row wins:
	for(int i = 0; i < 3; i++)
	{
		if(board[i][col] != marker)
			break;
		if (i == 2)
			return (player == 0) ? HOST_WON : CLIENT_WON;
	}

	// Check diagonals
	if(row == col)
	{
		for(int i = 0; i < 3; i++)
		{
			if(board[i][i] != marker)
				break;
			if (i == 2)
				return (player == 0) ? HOST_WON : CLIENT_WON;
		}
	}

	else if(row + col == 2)
	{
		for(int i = 0; i < 3; i++)
		{
			if(board[i][(2 - 1) - i] != marker)
				break;
			if(i == 2)
				return (player == 0) ? HOST_WON : CLIENT_WON;
		}
	}

	else if(moveCount == (9 - 1))
		return DRAW;

	return 0;
}

int handleBoard(int boardErr, int sockfd)
{
	switch(boardErr)
	{
		case HOST_WON:
			exitHost(sockfd, "Player 0 won! Ending game...", 0xFE);
			return 1;
			break;

		case CLIENT_WON:
			exitHost(sockfd, "Player 1 won! Ending game...", 0xFE);
			return 1;
			break;

		case DRAW:
			exitHost(sockfd, "The game has reached a stalemate. Ending game...", 0xFE);
			return 1;
			break;
	}

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
				char inputBuf[5];

				handleErr(boardErr);
				printf("Enter the row and column as 'c, r' or 'X' to exit:\n");
				
				fgets(inputBuf, sizeof(inputBuf), stdin);

				if(inputBuf[0] == 'X' || inputBuf[0] == 'x')
				{
					exitHost(sockfd, "Control sequence received. Exiting...\n", 0xFF);
					return 0;
				}

				if(inputBuf[5 - 1] != '\n')
				{
					char c;
					while((c = getchar()) != '\n' && c != EOF);
				}

				sscanf(inputBuf, "%hhu, %hhu", &col, &row);

			}while((boardErr = placeMarker(row, col, player, board)) < 0);


			code = encode(row, col, player);

			if(send(sockfd, (unsigned char*)&code ,sizeof(unsigned char), 0) == -1)
			{
				perror("Error in send()");
				return -1;
			}


			playerTurn = !playerTurn;
			if(handleBoard(boardErr, sockfd) == 1)
				break;
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

			else if(buf == 0xFE)
			{
				printf("Player %d has won! Ending game...\n", !player);
				return 0;
			}

			else if(buf == 0xFD)
			{
				printf("The game has reached a stalemate. Ending game...\n");
				return 0;
			}

			struct packet p = decode(buf);
			
			boardErr = placeMarker(p.row, p.col, !player, board);
			playerTurn = !playerTurn;
			if(handleBoard(boardErr, sockfd) == 1)
				break;
		}


	}

	return 0;
}

void exitHost(int sockfd, const char *msg, char exit)
{
	printf("%s\n", msg);

	if(send(sockfd, (unsigned char*)&exit, sizeof(unsigned char), 0) == -1)
	{
		perror("Could not confirm exit.\n");
	}

	close(sockfd);
}
