/**
 * @file:   usr_procA.c
 * @brief:  Two user processes: proc1...2 to do request empty memory
 * @author: G031
 * @date:   2016/02/03
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 * Expected UART output:

 * abcde
 * fghij
 * abcde
 * Process C
 * fghij
 * Proc 1 end of testing
 * Process C
 * Process C
 * Process C
 * 
 */

#include "assert.h"
#include "string.h"
#include "stdlib.h"
#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_stack_size=USR_SZ_STACK;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[0].m_priority   = MEDIUM;
	
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[1].m_priority   = LOW;
	
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[2].m_priority   = LOW;
	
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[3].m_priority   = LOW;
	
	g_test_procs[4].mpf_start_pc = &proc5;
	g_test_procs[4].m_priority   = LOW;
	
	g_test_procs[5].mpf_start_pc = &proc6;
	g_test_procs[5].m_priority   = LOW;

}

/**
 * @brief: a process that prints 2x5 lowercase letters
 */
void proc1(void)
{
	int i = 0;
	int j;
	int counter = 0;
	int ret_val = 100;
	char *c;
	void *mem;
	MSG_BUF *msg;
	MSG_BUF *msg2;
	
	while ( 1 ) {
		
		if (i == 5) {
			msg = (MSG_BUF *)request_memory_block();
			strcpy(msg->mtext, "%Z");
			msg->mtype = DEFAULT;
			msg2 = (MSG_BUF *)request_memory_block();
			strcpy(msg2->mtext, "%C 9 2");
			msg2->mtype = DEFAULT;
			send_message(PID_A, msg);
			delayed_send(PID_SET_PRIO, msg2, ONE_SECOND*25);
		}
		
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");
			counter++;
			if ( counter == 4 ) {
				//ret_val = set_process_priority(PID_P2, HIGH);
				break;
			} else {
				ret_val = release_processor();
			}
#ifdef DEBUG_0
			printf("proc1: ret_val = %d \n", ret_val);
#endif /* DEBUG_0 */
		}
		
		uart0_put_char('a' + i%10);
		i++;
	}
	c = "proc1 end of testing\n\r";
	uart0_put_string(c);
	while ( 1 ) {
		release_processor();
	}
}

/**
 * @brief: a process that prints 4x5 numbers 
 */
void proc2(void)
{
	while ( 1 ) {
		release_processor();
	}
}

void proc3(void) {
	while(1) {
		release_processor();
	}
}

void proc4(void) {
	while(1) {
		release_processor();
	}
}

void proc5(void)
{
	while ( 1 ) {
		release_processor();
	}
}

void proc6(void)
{
	while ( 1 ) {
		release_processor();
	}
}

