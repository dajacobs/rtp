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

// Initialize function
init() {
	// Pointer variables
	int i;
	float sum, avg;
	float jimsrand();
	// Get user input
	printf("-----  Stop and Wait Network Simulator Version -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d", &nsimmax);
    printf("Enter  packet loss probability [enter 0.0 for no loss]:");
    scanf("%f", &lossprob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f", &corruptprob);
    printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
    scanf("%f", &lambda);
    printf("Enter TRACE:");
    scanf("%d", &TRACE);
    // Random number generator
    srand(9999);              
    // Test random number generator
    sum = 0.0;               
    for(i = 0; i < 1000; i++) {
    	sum=sum+jimsrand(); 
    }
    avg = sum/1000.0;
    // Conditional print out to user
    if(avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n" ); 
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit();
    }

    // Initialize variables
    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;
    time = 0.0;
    generate_next_arrival(); 
}

// Random number function
float jimsrand() {
	// Largest int
	double mmm = 2147483647;
	// Pointer variable
	float x;
	// Initialize variable and return
	x = rand()/mmm;
	return(x);
}

/* Event Handle Routines */
// Next arrival function
generate_next_arrival() {
	// Pointer variables
	double x, log(), ceil();
	// Event pointer structure
	struct event *evptr;
	// Allocate memory 
	char *malloc();
	// Time variable
	float ttime;
	// Temporary int variable
	int tempint;

	if(TRACE > 2) {
		printf("Generate Next Arrival: creating new arrival\n");
	}
	
	// Initialize variables
	x = lambda*jimsrand()*2;
	evptr = (struct event *)malloc(sizeof(struct event));
	evptr->evtime = time + x;
	evptr->evtype = FROM_LAYER5;

	if(BIDIRECTIONAL && (jimsrand() > 0.5)) {
		evptr->eventity = B;
	} else {
		evptr->eventity = A;
	}
	insertevent(evptr);
}