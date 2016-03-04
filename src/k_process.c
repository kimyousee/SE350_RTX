/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * @author: Yiqing Huang
 * @author: Thomas Reidemeister
 * @date:   2014/02/28
 * NOTE: The example code shows one way of implementing context switching.
 *       The code only has minimal sanity check. There is no stack overflow check.
 *       The implementation assumes only two simple user processes and NO HARDWARE INTERRUPTS. 
 *       The purpose is to show how context switch could be done under stated assumptions. 
 *       These assumptions are not true in the required RTX Project!!!
 *       If you decide to use this piece of code, you need to understand the assumptions and
 *       the limitations. 
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "uart_polling.h"
#include "k_process.h"
#include "k_memory.h"
#include "priority_queue.h"
#include "linkedList.h"
#include "timer.h"
#include "system_proc.h"
#include "real_user_proc.h"
#include "k_message.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process = NULL; /* always point to the current RUN process */
PCB *blocked_receive_head = NULL;
PCB *blocked_receive_tail = NULL;

U32 g_switch_flag = 0;          /* whether to continue to run the process before the UART receive interrupt */
                                /* 1 means to switch to another process, 0 means to continue the current process */
				/* this value will be set by UART handler */

/* process initialization table */
PROC_INIT g_proc_table[TOTAL_PROCS];

extern PROC_INIT g_test_procs[NUM_TEST_PROCS];
extern PROC_INIT g_user_procs[NUM_USER_PROCS];
extern PROC_INIT g_system_procs[NUM_SYSTEM_PROCS];
extern uint32_t g_timer_count;
extern uint8_t g_char_in;

PQ *ready_queue;
LinkedList *timeout_queue;

void blocked_receive_print(PCB *head) {
	PCB *temp = head;
	while (temp != NULL) {
		printf("Process %d at memory 0x%x\n\r", temp->m_pid, temp);
		temp = temp->next;
	}
}

PCB *findPCB(U32 id) {
	int i;
	for (i=0; i<TOTAL_PROCS; i++) {
		if (gp_pcbs[i]->m_pid == id) {
			return gp_pcbs[i];
		}
	}
	return NULL;
}

void nullProc() {
	while(1){
		#ifdef DEBUG_0 
		printf("NULL proc running\n");
		#endif /* ! DEBUG_0 */
		k_release_processor();
	}
}

void UART_i_Proc() {
	PCB *pcb;
	MSG_BUF *msg;
	Node *node;
	#ifdef DEBUG_0 
	printf("UART proc running\n");
	#endif /* ! DEBUG_0 */
	pcb = findPCB(PID_UART_IPROC);
	if (g_char_in != 0) {
		node = (Node*)k_nonblocking_request_memory_block();
		msg = (MSG_BUF *)node;
		msg->m_send_pid = PID_UART_IPROC;
		msg->mtype = KEYBOARD_INPUT;
		msg->mtext[0] = g_char_in;
		g_char_in = 0;
		send_message(PID_KCD, PID_UART_IPROC, msg);
	}
	msg = receive_message_nonblocking(pcb);
	while (msg != NULL) {
		if (msg->m_send_pid == PID_CRT) {
			uart0_put_string((unsigned char *)msg->mtext);
			k_release_memory_block((void *)msg);
		}
		msg = receive_message_nonblocking(pcb); 
	}
}

void Timer_i_Proc() {
	PCB *pcb;
	void *mem;
	Node *node;
	MSG_BUF *msg;
	pcb = findPCB(PID_TIMER_IPROC);
	msg = receive_message_nonblocking(pcb);
	while (msg != NULL) {
		switch (msg->mtype) {
			case UPDATE_TIME:
				msg->mtext[0] = g_timer_count;
				k_send_message(msg->m_send_pid, msg);
				break;
			default:
				node = (Node*)k_nonblocking_request_memory_block();
				if (node == NULL) {
					break;
				}
				node->value = (int)(g_timer_count+(uint32_t)(msg->m_kdata[0]));
				node->message = (void *)msg;
				sortPushLinkedList(timeout_queue, node);
				break;
		}
		msg = receive_message_nonblocking(pcb);
	}
	while (linkedListHasNext(timeout_queue) && timeout_queue->head->value <= g_timer_count){
		MSG_BUF *envelope = (MSG_BUF*)(popLinkedList(timeout_queue)->message);
		int target_pid = envelope->m_recv_pid;
		k_send_message(target_pid, envelope);
	}
}

void addProcTable(int i, int id, int stack_size, int priority, void (*pc) ()) {
	g_proc_table[i].m_pid =id;
	g_proc_table[i].m_stack_size = stack_size;
	g_proc_table[i].mpf_start_pc = pc;
	g_proc_table[i].m_priority = priority;
}

/**
 * @biref: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void process_init() 
{
	int i;
	int j;
	U32 *sp;
        /* fill out the initialization table */
	set_test_procs();
	set_user_procs();
	set_system_procs();
	j = 0;
	// User processes
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		addProcTable(j, g_test_procs[i].m_pid, g_test_procs[i].m_stack_size, 
			g_test_procs[i].m_priority, g_test_procs[i].mpf_start_pc);
		j++;
	}
	
	for ( i = 0; i < NUM_USER_PROCS; i++ ) {
		addProcTable(j, g_user_procs[i].m_pid, g_user_procs[i].m_stack_size, 
			g_user_procs[i].m_priority, g_user_procs[i].mpf_start_pc);
		j++;
	}
	
	// KCD and CRT processes
	for ( i = 0; i < NUM_SYSTEM_PROCS; i++ ) {
		addProcTable(j, g_system_procs[i].m_pid, g_system_procs[i].m_stack_size, 
			g_system_procs[i].m_priority, g_system_procs[i].mpf_start_pc);
		j++;
	}
	
	// NULL process
	addProcTable(j, 0, 0x100, LOWEST+1, &nullProc);
	j++;
	
	// UART I Process
	addProcTable(j, PID_UART_IPROC, 0x100, LOWEST+1, &UART_i_Proc);
	j++;
	
	// Timer I Process
	addProcTable(j, PID_TIMER_IPROC, 0x100, LOWEST+1, &Timer_i_Proc);
  j++;
	
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < TOTAL_PROCS; i++ ) {
		int j;
		(gp_pcbs[i])->m_pid = (g_proc_table[i]).m_pid;
		//printf("pid = %d, start = %d\n", (g_proc_table[i]).m_pid, (g_proc_table[i]).mpf_start_pc);
		(gp_pcbs[i])->m_state = NEW;
		(gp_pcbs[i])->m_priority = (g_proc_table[i]).m_priority;
		
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		
		printf("stack = 0x%x\n", sp);
		
		if (!isIProcess(gp_pcbs[i]->m_pid)) {
			pq_push(ready_queue, gp_pcbs[i]);
		}
		
		(gp_pcbs[i])->mp_sp = sp;
	}
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 *         NULL if error happens
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */

PCB *scheduler(void)
{
	PCB *ret;
	ret = pq_pop(ready_queue);
	return  ret;
}

int isIProcess(U32 id) {
	if (id == PID_TIMER_IPROC || id == PID_UART_IPROC) {
		return 1;
	}
	return 0;
}

/*@brief: switch out old pcb (p_pcb_old), run the new pcb (gp_current_process)
 *@param: p_pcb_old, the old pcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_pcb_old and gp_current_process are pointing to valid PCBs.
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
int process_switch(PCB *p_pcb_old) 
{
	PROC_STATE_E state;
	#ifdef DEBUG_0 
	printf("switching from process %d to process %d\n", p_pcb_old->m_pid, gp_current_process->m_pid);
	#endif /* ! DEBUG_0 */
	state = gp_current_process->m_state;

	if (state == NEW) {
		if (gp_current_process != p_pcb_old && p_pcb_old->m_state != NEW) {
			p_pcb_old->mp_sp = (U32 *) __get_MSP();
			if (p_pcb_old->m_state != BLK && p_pcb_old->m_state != BLK_RCV) {
				p_pcb_old->m_state = RDY;
				pq_push(ready_queue, p_pcb_old);
			}
		}
		gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp);
		__rte();  // pop exception stack frame from the stack for a new processes
	} 
	
	/* The following will only execute if the if block above is FALSE */

	if (gp_current_process != p_pcb_old) {
		if (state == RDY){
			p_pcb_old->mp_sp = (U32 *) __get_MSP(); // save the old process's sp
			if (p_pcb_old->m_state != BLK && p_pcb_old->m_state != BLK_RCV) {
				p_pcb_old->m_state = RDY;
				pq_push(ready_queue, p_pcb_old);
			}
			gp_current_process->m_state = RUN;
			__set_MSP((U32) gp_current_process->mp_sp); //switch to the new proc's stack    
		} else {
			gp_current_process = p_pcb_old; // revert back to the old proc on error
			return RTX_ERR;
		} 
	}
	return RTX_OK;
}
/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void)
{
	PCB *p_pcb_old = NULL;
	
	if (gp_current_process != NULL && gp_current_process->m_priority < pq_peak(ready_queue)->m_priority && gp_current_process->m_state!=BLK && gp_current_process->m_state!=BLK_RCV) {
	#ifdef DEBUG_0 
		//printf("remaining on process %d\n", gp_current_process->m_pid);
	#endif /* ! DEBUG_0 */
		return RTX_OK;
	}
	
	p_pcb_old = gp_current_process;
	gp_current_process = scheduler();
	
	if ( gp_current_process == NULL  ) {
		gp_current_process = p_pcb_old; // revert back to the old process
		return RTX_ERR;
	}
	
  if ( p_pcb_old == NULL) {
		p_pcb_old = gp_current_process;
	}
	
	process_switch(p_pcb_old);
	return RTX_OK;
}

PCB* get_process(int pid, PCB **queue) {
	int i;
	
	for (i = 0; i < TOTAL_PROCS; i++ ) {
		if (queue[i]->m_pid == pid){
			return queue[i];
		}
	}
	return NULL;
}

int k_get_process_priority(int pid){
	int i;
	PCB *p = get_process(pid, gp_pcbs);
	#ifdef DEBUG_0 
	printf("getting priority for process %d\n", p->m_pid);
	#endif /* ! DEBUG_0 */
	if (p == NULL) {
		return -1;
	}
	return p->m_priority;
}

void check_priority(void){
	if (gp_current_process->m_priority > pq_peak(ready_queue)->m_priority) {
		#ifdef DEBUG_0 
		printf("current process %d preempted\n", gp_current_process->m_pid);
		#endif /* ! DEBUG_0 */
		k_release_processor();
	}
}

void k_set_process_priority(int pid, int prio){
	int i;
	PCB *p;
	
	if (pid == PID_NULL || prio < HIGH || prio > LOWEST) {
		return;
	}
		
	p = get_process(pid, gp_pcbs);
	if (p != NULL) {
		#ifdef DEBUG_0 
		printf("setting process %d priority from %d to %d\n", p->m_pid, p->m_priority, prio);
		#endif /* ! DEBUG_0 */
		p->m_priority = prio;
	}
	
	pq_sort(ready_queue);
	check_priority();
	
}
