#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

// use
void pq_init (PQUEUE *pq, int num_pcb) {
	pq->len = 0;
}

void pq_push (PQUEUE *pq, PCB *pcb) {
	int i = pq->len + 1;
	int j = i / 2;
	while (i > 1 && pq->pq_pcbs[j]->m_priority > pcb->m_priority) {
		pq->pq_pcbs[i] = pq->pq_pcbs[j];
		i = j;
		j = j / 2;
  }
	pq->pq_pcbs[i] = pcb;
	pq->len+=1;
}

PCB *pq_pop (PQUEUE *pq) {
	int i,j,k;
	PCB *ret;
	if (pq->len <= 0) {
		return NULL;
	}
	ret = pq->pq_pcbs[0];
	pq->pq_pcbs[0] = pq->pq_pcbs[pq->len];
	pq->len--;
	i = 1;
	while(1) {
		k = i;
		j = 2 * i;
		if (j <= pq->len && pq->pq_pcbs[j]->m_priority < pq->pq_pcbs[k]->m_priority) {
            k = j;
        }
        if (j + 1 <= pq->len && pq->pq_pcbs[j + 1]->m_priority < pq->pq_pcbs[k]->m_priority) {
            k = j + 1;
        }
        if (k == i) {
            break;
        }
        pq->pq_pcbs[i] = pq->pq_pcbs[k];
        i = k;
	}
	pq->pq_pcbs[i] = pq->pq_pcbs[pq->len + 1];
	return ret;
	
}
