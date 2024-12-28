#include "packet.h"
#include <stdio.h>

unsigned char encode(unsigned char row, unsigned char col, unsigned char player)
{
	unsigned char code = row;
	
	code <<= 2;

	code |= col;

	code <<= 2;

	code |= player;

	return code;
}

struct packet decode(unsigned char code)
{
	struct packet p;

	p.row = code >> 4;
	p.col = (code >> 2) % 0x4;
	p.player = code % 0x2;
	
	return p;
}
