#include <stdio.h>
#define BIDIRECTIONAL 1

// Character message data structure
struct msg {
	char data[20];
};

// Packet structure
struct pkt {
	int seqnum;
	int acknum;
	int checksum;
	char payload[20];
};

/* Network emulation */

// Event structure
struct event {
	// Event time
	float evtime;
	// Event type code
	int evtype;
	// Event entity of occurence
	int eventity;
	// Pointer to packet with event
	struct pkt *pktptr;
	// Previous event occured
	struct event *prev;
	// Next event occured
	struct event *next;
};