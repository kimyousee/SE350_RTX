#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

#include "printf.h"

void pq_push (PQ * pq, PCB *pcb) {
	int i = pq->len;
	int j = (i-1);
	pq->p_queue[i] = pcb;

	while (i > 0 && pq->p_queue[j]->m_priority > pq->p_queue[i]->m_priority) {
		PCB *temp = pq->p_queue[i];
		pq->p_queue[i] = pq->p_queue[j];
		pq->p_queue[j] = temp;
		
		i = j;
		j = (i-1);
  }
	
	pq->len+=1;
}

void pq_sort(PQ *pq) {
	int i = pq->len-1;
	int j = (i-1);
	while (i > 0) {
		
		if (pq->p_queue[j]->m_priority > pq->p_queue[i]->m_priority) {
			PCB *temp = pq->p_queue[i];
			pq->p_queue[i] = pq->p_queue[j];
			pq->p_queue[j] = temp;
		}
		
		i = j;
		j = (i-1);
  }
}

PCB *pq_peak (PQ *pq) {
	return pq->p_queue[0];
}

PCB *pq_pop (PQ *pq) {
	int i;
	PCB *ret;
	if (pq->len <= 0) {
		return NULL;
	}
	ret = pq->p_queue[0];
	pq->len--;
	
	for (i = 0; i < pq->len; i++){
		pq->p_queue[i] = pq->p_queue[i+1];
	}
	 pq->p_queue[pq->len] = NULL;
	
	return ret;
	
}

void pq_print(PQ *pq) {
	int i;
	int len = pq->len;
	if (len == 0) {
		printf("Queue is empty\n\r");
		return;
	}
	for (i=0; i<len; i++) {
		printf("Process %d is in memory 0x%x \n\r", pq->p_queue[i]->m_pid, pq->p_queue[i]);
	}
}
