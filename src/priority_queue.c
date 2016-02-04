#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

void pq_push (PCB *pcb) {
	int i = p_queue_len;
	int j = (i-1) / 2;
	p_queue[i] = pcb;
	
	while (i >= 0 && p_queue[j]->m_priority > p_queue[i]->m_priority) {
		PCB *temp = p_queue[i];
		p_queue[i] = p_queue[j];
		p_queue[j] = temp;
		
		i = j;
		j = (j-1) / 2;
  }
	p_queue_len+=1;
}

PCB *pq_peak () {
	return p_queue[0];
}

PCB *pq_pop () {
	//NUM_TEST_PROCS
	
	int i,j,k;
	PCB *ret;
	if (p_queue_len <= 0) {
		return NULL;
	}
	ret = p_queue[0];
	p_queue[0] = p_queue[p_queue_len];
	p_queue_len--;
	
	i = 0;
	while(1) {
		k = i;
		j = 2 * i;
		if (j <= p_queue_len && p_queue[j]->m_priority < p_queue[k]->m_priority) {
            k = j;
        }
        if (j + 1 <= p_queue_len && p_queue[j + 1]->m_priority < p_queue[k]->m_priority) {
            k = j + 1;
        }
        if (k == i) {
            break;
        }
        p_queue[i] = p_queue[k];
        i = k;
	}
	p_queue[i] = p_queue[p_queue_len + 1];
	return ret;
	
}
