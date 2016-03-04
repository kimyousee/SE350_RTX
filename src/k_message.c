#include "k_message.h"
#include "printf.h"

extern void UART_i_Proc(void);

void enqueue_block_receive(PCB *pcb) {
	if (blocked_receive_head == NULL) {
		pcb->next = NULL;
		blocked_receive_head = pcb;
		blocked_receive_tail = pcb;
		return;
	}
	blocked_receive_tail->next = pcb;
	blocked_receive_tail = blocked_receive_tail->next;
	blocked_receive_tail->next = NULL;
}

PCB* dequeue_block_receive(U32 pid) {
	PCB *n = blocked_receive_head;
	PCB *p = NULL;
	if (blocked_receive_head == NULL) {
		return NULL;
	}
	
	if (n->m_pid == pid) {
		blocked_receive_head = n->next;
		return n;
	}
	
	if (n->next == NULL) {
		blocked_receive_head = NULL;
		blocked_receive_tail = NULL;
		return NULL;
	}
	while (n->next != NULL && n->next->m_pid != pid) {
			n = n->next;
	}
	if (n->next == NULL) {
		return NULL;
	}
	p = n->next;
	n->next = p->next;
	return p;
}

void enqueue_msg(PCB *p, MSG_BUF *msg) {
	if (p->head_msg == NULL) {
		msg->mp_next = NULL;
		p->head_msg = msg;
		p->tail_msg = msg;
		return;
	}
	p->tail_msg->mp_next = msg;
	p->tail_msg = p->tail_msg->mp_next;
	p->tail_msg->mp_next = NULL;
}

MSG_BUF* dequeue_msg(PCB *p) {
	MSG_BUF *n = p->head_msg;
	if (p->head_msg == NULL) {
		return NULL;
	}
	p->head_msg = p->head_msg->mp_next;
	return n;
}

void k_send_message(int receiving_pid, MSG_BUF *msg) {
	__disable_irq();
	send_message(receiving_pid, gp_current_process->m_pid, msg);
	__enable_irq();
}

void send_message(int receiving_pid, int sending_pid, MSG_BUF *msg) {
	PCB *p;
	int i;
	//atomic = TRUE;
	//if (msg->m_send_pid == 0) {
	msg->m_send_pid = sending_pid;
	//}
	msg->m_recv_pid = receiving_pid;
	
	p = get_process(receiving_pid, gp_pcbs);
	if (p == NULL){
		return;
	}
	#ifdef DEBUG_0 
	printf("process %d sending message(%s) to process %d\n", msg->m_send_pid, msg->mtext, p->m_pid);
	#endif /* ! DEBUG_0 */
	enqueue_msg(p, msg);
	if (receiving_pid == PID_UART_IPROC) {
		UART_i_Proc();
	} else {
		if (p->m_state == BLK_RCV) {
			#ifdef DEBUG_0 
			printf("process %d unblocked on receive\n", p->m_pid);
			#endif /* ! DEBUG_0 */
			dequeue_block_receive(p->m_pid);
			p->m_state = RDY;
			pq_push(ready_queue, p);
			check_priority();
		}
	}
	
	//atomic = FALSE;
}

void *receive_message_nonblocking(PCB *p) {
	MSG_BUF *msg = dequeue_msg(p);
	return (void *)msg;
}

void *k_receive_message(int *p_pid) {
	MSG_BUF *msg;
	#ifdef DEBUG_0 
	printf("process %d attempting to receive messages\n", gp_current_process->m_pid);
	#endif /* ! DEBUG_0 */
	while(gp_current_process->head_msg == NULL) {
		#ifdef DEBUG_0 
		printf("process %d blocked on receive\n", gp_current_process->m_pid);
		#endif /* ! DEBUG_0 */
		gp_current_process->m_state = BLK_RCV;
		enqueue_block_receive(gp_current_process);
		k_release_processor();
	}
	msg = dequeue_msg(gp_current_process);
	return (void *)msg;
}

void k_delayed_send(int pid, void *p_msg, int delay) {
	PCB *p;
	MSG_BUF *msg = (MSG_BUF*) p_msg;
	
	msg->m_send_pid = gp_current_process->m_pid;
	msg->m_recv_pid = pid;
	msg->m_kdata[0] = (int)delay;
	
	p = get_process(PID_TIMER_IPROC, gp_pcbs);
	
	if (p == NULL){
		return;
	}
	#ifdef DEBUG_0 
	printf("process %d sending message(%s) to process %d\n", gp_current_process->m_pid, msg->mtext, p->m_pid);
	#endif /* ! DEBUG_0 */
	enqueue_msg(p, msg);
}
