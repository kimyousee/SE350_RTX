/**
 * @file:   usr_procA.c
 * @brief:  Two user processes: proc1...2 to do request empty memory
 * @author: G031
 * @date:   2016/02/03
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 * Expected UART output:
 * abcde
 * 01234
 * fghij
 * 56789abcde
 * 
 * fghij
 * proc1 end of testing
 * 01234
 * 56789
 * proc2 end of testing

 *
 * 
 */

#include "rtx.h"
#include "string.h"
#include "stress_test_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_stress_test_procs[NUM_STRESS_TEST_PROCS];

void set_stress_test_procs() {
	int i;
	for( i = 0; i < NUM_STRESS_TEST_PROCS; i++ ) {
		g_stress_test_procs[i].m_pid=(U32)(i+1);
		g_stress_test_procs[i].m_stack_size=USR_SZ_STACK;
	}
  
	g_stress_test_procs[0].m_pid=PID_A;
	g_stress_test_procs[0].mpf_start_pc = &procA;
	g_stress_test_procs[0].m_priority = MEDIUM;
	
	g_stress_test_procs[1].m_pid=PID_B;
	g_stress_test_procs[1].mpf_start_pc = &procB;
	g_stress_test_procs[1].m_priority = MEDIUM;
	
	g_stress_test_procs[2].m_pid=PID_C;
	g_stress_test_procs[2].mpf_start_pc = &procC;
	g_stress_test_procs[2].m_priority = HIGH;
	
}

void procA(void) {
	void *mem;
	MSG_BUF *p_msg_env;
	int num = 0;
	mem = request_memory_block();
	p_msg_env = (MSG_BUF *)mem;
	p_msg_env->mtype = KCD_REG;
	p_msg_env->mtext[0] = '%';
	p_msg_env->mtext[1] = 'Z';
	p_msg_env->mtext[2] = '\0';
	send_message(PID_KCD, (void *)p_msg_env);
	
	while(1) {
		MSG_BUF *receive_msg;
		//printf("ProcA running\n\r");
		receive_msg = receive_message(NULL);
		if (receive_msg->mtext[0] == '%' && receive_msg->mtext[1] == 'Z') {
			release_memory_block((void *)receive_msg);
			break;
		} else {
			release_memory_block((void *)receive_msg);
		}
	}
	while (1) {
		MSG_BUF *send_msg;
		mem = request_memory_block();
		send_msg = (MSG_BUF *)mem;
		send_msg->mtype = COUNT_REPORT;
		send_msg->m_kdata[0] = num;
		send_message(PID_B, send_msg);
		num = num + 1;
		release_processor();
	}
}

void procB(void)
{
	MSG_BUF *msg;
	while ( 1 ) {
		msg = receive_message(NULL);
		send_message(PID_C, msg);
		//printf("ProcB running\n\r");
		release_processor();
	}
}

void enqueue_local_msg(MSG_BUF **head, MSG_BUF **tail, MSG_BUF *msg) {
	if (*head == NULL) {
		msg->mp_next = NULL;
		*head = msg;
		*tail = msg;
		return;
	}
	(*tail)->mp_next = msg;
	(*tail) = (*tail)->mp_next;
	(*tail)->mp_next = NULL;
}

MSG_BUF* dequeue_local_msg(MSG_BUF **head, MSG_BUF **tail) {
	MSG_BUF *n = *head;
	if (*head == NULL) {
		return NULL;
	}
	(*head) = (*head)->mp_next;
	return n;
}

void procC(void)
{
	MSG_BUF *msg_queue_head;
	MSG_BUF *msg_queue_tail;
	MSG_BUF *msg;
	int printFlag = 0;
	msg_queue_head = NULL;
	msg_queue_tail = NULL;
	while ( 1 ) {
		//printf("ProcC running\n\r");
		if (msg_queue_head == NULL) {
			msg = receive_message(NULL);
		} else {
			msg = dequeue_local_msg(&msg_queue_head, &msg_queue_tail);
		}
		
		//printf("Message type: %d\n\r", msg->mtype);
		if (msg->mtype == COUNT_REPORT) {
			//printf("Report count: %d\n\r", msg->m_kdata[0]);
			if (msg->m_kdata[0] % 20 == 0) {
				msg->mtype = CRT_DISPLAY;
				strcpy(msg->mtext, "Process C\n\r");
				send_message(PID_CRT, msg);
				printFlag = 1;
				
				// Hibernate
				msg = (MSG_BUF *)request_memory_block();
				msg->mtype = WAKEUP10;
				delayed_send(PID_C, msg, 100);
				while (1) {
					msg = receive_message(NULL);
					//printf("Proc C woke up %d\n\r", msg->mtype);
					if (msg->mtype == WAKEUP10) {
						break;
					} else {
						enqueue_local_msg(&msg_queue_head, &msg_queue_tail, msg);
					}
				}	
			}
		}
		if (printFlag == 0) {
			release_memory_block(msg);
		}
		printFlag = 0;
		release_processor();
	}
}

