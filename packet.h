#ifndef PACKET_H
#define PACKET_H

struct packet 
{
	unsigned char row;
	unsigned char col;
	unsigned char player;
};

unsigned char encode(unsigned char row, unsigned char col, unsigned char player);
struct packet decode(unsigned char);

#endif // !DEBUG
