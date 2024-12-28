#ifndef BOARD_H
#define BOARD_H
#include <stdio.h>
#include "consts.h"
#include <stdbool.h>

void drawBoard(char board[3][3]);
int placeMarker(int row, int col, int player, char board[3][3]);
int gameLoop(bool IS_GAME_RUNNING, bool player, int sockfd, char board[3][3]);

#endif // !DEBUG
