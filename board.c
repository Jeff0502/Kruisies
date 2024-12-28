#include "board.h"
#include <sys/socket.h>
#include <stdbool.h>
#include "packet.h"

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
	int read_result;

	while(IS_GAME_RUNNING)
	{
		if(playerTurn == player)
		{
			printf("Enter the row and column [r, c]:\n");
			scanf(" [%hhu, %hhu]", &row, &col);

			code = encode(row, col, player);

			if(send(sockfd, (unsigned char*)&code ,sizeof(unsigned char), 0) == -1)
			{
				perror("Error in send()");
			}

			placeMarker(row, col, player, board);

			playerTurn = !playerTurn;
		}

		else 
		{
			printf("Waiting for player %d...", !player);

			if((read_result = recv(sockfd, (unsigned char*)&buf, sizeof(unsigned char), 0)) == -1)
			{
				perror("Client receive");
				perror(NULL);
			}

			struct packet p = decode(buf);
			
			placeMarker(p.row, p.col, !player, board);
			playerTurn = !playerTurn;
		}
	}

	return 0;
}

