#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BIDIRECTIONAL 1
#define TIMER_INTERRUPT 0
#define FROM_LAYER5 1
#define FROM_LAYER3 2
#define OFF 0
#define ON 1
#define A 0
#define B 1

/* Structures */

// Message 
struct msg {
	char data[20];
};
// Packet
struct pkt {
	int seqnum;
	int acknum;
	int checksum;
	char payload[20];
};
// Event 
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
// Initialize structures
struct event *evlist = NULL;
struct pkt *packet;
struct event *p;

/* Variables */

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
// Pointer a or b
int AorB;
// Data sent 
char datasent[20];
// Increment
float increment;

// Prototypes
void init(void);
float jimsrand(void);
void generate_next_arrival(void);
void insertevent(struct event *p);
void printevlist(void);
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void tolayer3(int AorB, struct pkt packet);
void tolayer5(int AorB, char datasent[]);

/* Functions */

// Initialize
void init(void) {
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
    exit(EXIT_FAILURE);
    }
    // Initialize variables
    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;
    time = 0.0;
    generate_next_arrival(); 
}
// Random number
float jimsrand(void) {
	// Largest int
	double mmm = 2147483647;
	// Pointer variable
	float x;
	// Initialize variable and return
	x = rand()/mmm;
	return(x);
}
// Generate next arrival
void generate_next_arrival(void) {
	// Pointer variables
	double x, log(), ceil();
	// Event pointer structure
	struct event *evptr;
	// Time variable
	float ttime;
	// Temporary int variable
	int tempint;
	if(TRACE > 2) {
		printf("Generate Next Arrival: creating new arrival\n");
	}
	// Initialize variables
	x = lambda*jimsrand()*2;
	evptr = (struct event*)malloc(sizeof(struct event));
	evptr->evtime = time + x;
	evptr->evtype = FROM_LAYER5;
	if(BIDIRECTIONAL && (jimsrand() > 0.5)) {
		evptr->eventity = B;
	} else {
		evptr->eventity = A;
	}
	insertevent(evptr);
}
// Insert event
void insertevent(struct event *) {
	// Event structure
	struct event *q, *qold;
	if(TRACE > 2) {
      printf("INSERTEVENT: time is %lf\n", time);
      printf("INSERTEVENT: future time will be %lf\n", p->evtime); 
    }
    // Header of list
	q = evlist;
	// Empty list     
   	if(q == NULL) {
   		evlist = p;
    	p->next = NULL;
    	p->prev = NULL;
    } else {
    	for(qold = q; q != NULL && p->evtime > q->evtime; q = q->next) {
        	qold = q; 
    	}
    	// End of list
        if(q == NULL) {
        	qold->next = p;
            p->prev = qold;
            p->next = NULL;
        // Front of list    
        } else if(q == evlist) {
        	p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
        	evlist = p;
        // Middle of list
        } else {
        	p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}
// Print event list
void printevlist(void) {
	// Event structure
	struct event *q;
	// Pointer variable
  	int i;
  	printf("--------------\nEvent List Follows:\n");
  	for(q = evlist; q!=NULL; q=q->next) {
    	printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
    }
  	printf("--------------\n");
}
// Start timer
void starttimer(int AorB, float increment) {
	struct event *q;
	struct event *evptr;
 	if(TRACE > 2) {
    	printf("          START TIMER: starting timer at %f\n",time);
 	}
   	for(q = evlist; q != NULL ; q = q->next) {
   		if((q->evtype == TIMER_INTERRUPT  && q->eventity == AorB) ) { 
      		printf("Warning: attempt to start a timer that is already started\n");
      		return;
      	}
   	}
   	// Initialize event for timer  
	evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime =  time + increment;
    evptr->evtype =  TIMER_INTERRUPT;
    evptr->eventity = AorB;
   	insertevent(evptr);
}
// Stop timer
void stoptimer(int AorB) {
	struct event *q,*qold;
	if(TRACE > 2) {
		printf("STOP TIMER: stopping timer at %f\n", time);
	}
	for(q = evlist; q != NULL ; q = q->next) {
		if((q->evtype == TIMER_INTERRUPT  && q->eventity == AorB)) { 
       		// Remove first event
       		if(q->next == NULL && q->prev == NULL) {
             	evlist = NULL;
            // End of list 	
       		} else if (q->next == NULL) {
            	q->prev->next = NULL;
       		// Front of list
       		} else if (q == evlist) {
            	q->next->prev = NULL;
             	evlist = q->next;
            // Middle of the list
            } else {
            	q->next->prev = q->prev;
            	q->prev->next =  q->next;
            }
    		free(q);
      		return;
    	}
  	printf("Warning: unable to cancel your timer. It wasn't running.\n");
	} 
}
// Layer three
void tolayer3(int AorB, struct pkt packet) {
	struct pkt *mypktptr;
 	struct event *evptr,*q;
 	float lastime, x, jimsrand();
 	int i;
	ntolayer3++;
	// Simulate loss
 	if(jimsrand() < lossprob) {
    	  nlost++;
    	if(TRACE > 0) {
    		printf("TOLAYER3: packet being lost\n");
    	}    
		return;
    }  
    // Copy packet pointer
 	mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
 	mypktptr->seqnum = packet.seqnum;
 	mypktptr->acknum = packet.acknum;
 	mypktptr->checksum = packet.checksum;
 	for (i = 0; i < 20; i++) {
 		mypktptr->payload[i] = packet.payload[i];
 	}
 	if (TRACE>2)  {
   		printf("TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
	  	mypktptr->acknum,  mypktptr->checksum);
    	for (i = 0; i < 20; i++) {
    		printf("%c",mypktptr->payload[i]);
    	}
        printf("\n");
   }
   	// Future event initialize
 	evptr = (struct event *)malloc(sizeof(struct event));
  	evptr->evtype =  FROM_LAYER3; 
  	evptr->eventity = (AorB+1) % 2;
  	evptr->pktptr = mypktptr;      
 	lastime = time;
 	// Compute arrival time of packet
 	for(q = evlist; q != NULL ; q = q->next) {
    	if((q->evtype == FROM_LAYER3  && q->eventity == evptr->eventity)) {
    		lastime = q->evtime;
    	} 
    }
 	evptr->evtime =  lastime + 1 + 9*jimsrand();
 	// Simulate corruption 
	if(jimsrand() < corruptprob) {
		ncorrupt++;
		if((x = jimsrand()) < .75) {
			mypktptr->payload[0] = 'Z';
		} else if(x < .875) {
			mypktptr->seqnum = 999999;
		} else {
	       mypktptr->acknum = 999999;
		}
	    if(TRACE > 0) {
	    	printf("TOLAYER3: packet being corrupted\n");
	    }   
	}
	if(TRACE > 2) {
		printf("TOLAYER3: scheduling arrival on other side\n");
	}  
	insertevent(evptr);
} 
// Layer five
void tolayer5(int AorB, char datasent[]) {
	int i;  
	if (TRACE > 2) {
    	printf("TOLAYER5: data received: ");
    	for (i = 0; i < 20; i++) {
    		printf("%c", datasent[i]);		
    	}  
    	printf("\n");
   }
} 
// Main
main(void) {
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