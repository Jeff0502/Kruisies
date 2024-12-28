#include "board.h"

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

	return 0;
}

