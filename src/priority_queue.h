#include "k_process.h"

typedef struct PriQ {
	PCB **p_queue;
	int len;
} PQ;

void pq_push (PQ *pq, PCB *pcb);
PCB *pq_pop (PQ *pq);
PCB *pq_peak (PQ *pq);




