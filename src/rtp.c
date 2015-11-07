#include <stdio.h>
#define BIDIRECTIONAL 1

// Character message data
struct msg {
	char data[20];
};

// Packet function
struct pkt {
	int seqnum;
	int acknum;
	int checksum;
	char payload[20];
};