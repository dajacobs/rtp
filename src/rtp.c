#include <stdio.h>
#define BIDIRECTIONAL 1

/* Alternating Bit and Go-Back-N Network Emulation */

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

/* Network Emulation */

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

// Event list
struct event *evlist = NULL;

// Possible event occurences
#define TIMER_INTERRUPT 0
#define FROM_LAYER5 1
#define FROM_LAYER3 2
#define OFF 0
#define ON 1
#define A 0
#define B 1

// Trace for debug
int TRACE = 1;
// Number of message between layers
int nsim = 0;
// Number of messages generated
int nsimmax = 0;
// Time counter
float time = 0.000;
// Loss probability of packet
float lossprob;
// Flipped probability of packet
float corruptprob;
// Arrival rate of message between layers
float lambda;
// Number sent to layer
int ntolayer3;
// Number lost in media
int nlost;
// Number corrupted by media
int ncorrupt;

// Main function
main() {
	// Event pointer structure
	struct event *eventptr;
	// Message to give structure
	struct msg msg2give;
	// Packet to give structure
	struct pkt pkt2give;
	// Pointer variables
	int i,j;
	char c;
	// Initialize functions
	init();
	A_inut();
	B_init();
}