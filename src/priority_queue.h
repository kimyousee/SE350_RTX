#include "k_process.h"

typedef struct PQ{
	PCB **pq_pcbs;
	int len;
} PQUEUE;

void pq_init (PQUEUE *pq, int num_pcb);
void pq_push (PQUEUE *pq, PCB *pcb);
PCB *pq_pop (PQUEUE *pq);
