#include "k_process.h"

#ifndef _PRIORITY_QUEUE_
#define _PRIORITY_QUEUE_

typedef struct PriQ {
	PCB **p_queue;
	int len;
} PQ;

void pq_push (PQ *pq, PCB *pcb);
PCB *pq_pop (PQ *pq);
PCB *pq_peak (PQ *pq);
void pq_sort (PQ *pq);
void pq_print(PQ *pq);

#endif


