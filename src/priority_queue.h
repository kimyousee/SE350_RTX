#include "k_process.h"

void pq_push (PCB *pcb);
PCB *pq_pop ();

extern PCB **gp_pcbs;
extern PCB **p_queue;
extern int p_queue_len;
extern PCB *pq_peak ();