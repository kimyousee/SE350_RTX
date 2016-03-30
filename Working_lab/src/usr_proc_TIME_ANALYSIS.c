/**
 * @file:   usr_proc_TIME_ANALYSIS.c
 * @brief:  Time send_message, receive_message, and request_memory_block
 * @author: G031
 * @date:   2016/02/03
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "assert.h"
#include "string.h"
#include "stdlib.h"
#include "rtx.h"
#include "timer2.h"
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
	g_test_procs[0].m_priority   = HIGH;
	
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[1].m_priority   = MEDIUM;
	
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
 * @brief: time for sending a message
 */
void proc1(void)
{
	int i = 0;
	int j;
	int counter = 0;
	int ret_val = 100;
	char *c;
	void *mem;
	int time;
	int sum_time = 0;
	int time_array[10];
	int average_time = 0;
	int *pTime;
	struct msgbuf *p_msg_env;
	
	while ( 1 ) {
		
		if (i==5) {
			// Create message
			for (j = 0; j < 10; j++){
				p_msg_env = (struct msgbuf *) request_memory_block();
				p_msg_env->mtype = DEFAULT;
				p_msg_env->mtext[0] = 'M';
				p_msg_env->mtext[1] = 's';
				p_msg_env->mtext[2] = 'g';
				p_msg_env->mtext[3] = '\0';
				
				time = k_time_send_message(PID_P2, (void *)p_msg_env);
				time_array[j] = time;
				sum_time += time;
			}
			
			average_time = sum_time/10;
			printf("Average time for sending message: %d\n\r", average_time);
			set_process_priority(PID_P1, 4);
		}
		
		if (i==15) {
			pTime = (int*)request_memory_block();
			sum_time = 0;
			average_time = 0;
			for (j = 0; j < 10; j++){
				mem = k_time_request_memory_block(pTime);
				time_array[j] = *pTime;
				sum_time += *pTime;
			}
			average_time = sum_time/10;
			printf("Average time for requesting memory block: %d\n\r", average_time);
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
	int i = 0;
	int j = 0;
	int ret_val = 20;
	int counter = 0;
	char* c;
	MSG_BUF *mem;
	int *pTime;
	int time = 0;
	int time_array[10];
	int sum_time = 0;
	int average_time = 0;
	
	while (1) {
		if ( i!=0 && i==10 ) {
			pTime = (int *) request_memory_block();
			// Will come here after proc1 has finished sending all messages
			for (j = 0; j < 10; j++){
				mem = (MSG_BUF *) k_time_receive_message((int *)PID_P2, pTime);
				time_array[j] = *pTime;
				sum_time += *pTime;
			}
			
			average_time = sum_time/10;
			printf("Average time for receiving message: %d\n\r", average_time);
			set_process_priority(PID_P1, 1);
		}
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");
			counter++;
			if ( counter == 4 ) {
				//ret_val = set_process_priority(PID_P1, HIGH);
				break;
			} else {
				ret_val = release_processor();
			}
#ifdef DEBUG_0
			printf("proc2: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
		}
		uart0_put_char('0' + i%10);
		i++;
	}
	c = "proc2 end of testing\n\r";
	uart0_put_string(c);
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

