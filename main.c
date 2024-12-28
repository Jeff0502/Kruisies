#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "connect.h"
#include "consts.h"
#include "board.h"

int main(int argc, char* argv[])
{
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
	//
	if(strcmp(argv[1], "server") == 0)
		serverDriver();
	else if(strcmp(argv[1], "client") == 0)
		clientDriver("127.0.0.1");

	return 0;
}
