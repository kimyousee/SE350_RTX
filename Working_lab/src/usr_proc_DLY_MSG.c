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
		g_test_procs[i].m_stack_size=0x300;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[0].m_priority   = MEDIUM;
	
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
	
//	while ( 1 ) {
		
// 		if ( i != 0 && i==15) {
// 			struct msgbuf *p_msg_env = (struct msgbuf *) request_memory_block();
// 			p_msg_env->mtype = KCD_REG;
// 			p_msg_env->mtext[0] = '%';
// 			p_msg_env->mtext[1] = 'W';
// 			p_msg_env->mtext[2] = '\0';
// 			//delayed_send(PID_P2, (void *)p_msg_env, 50);
// 		}
// 		
// 		//if (i!=0 && i==15) {
// 		//}
// 		
// 		if ( i != 0 && i%5 == 0 ) {
// 			uart0_put_string("\n\r");
// 			counter++;
// 			if ( counter == 8 ) {
// 				//ret_val = set_process_priority(PID_P2, HIGH);
// 				break;
// 			} else {
// 				ret_val = release_processor();
// 			}
// #ifdef DEBUG_0
// 			printf("proc1: ret_val = %d \n", ret_val);
// #endif /* DEBUG_0 */
// 		}
// 		
// 		uart0_put_char('a' + i%10);
// 		i++;
// 	}
// 	c = "proc1 end of testing\n\r";
// 	uart0_put_string(c);
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
	int ret_val = 20;
	int counter = 0;
	char* c;
	void *mem;
	struct msgbuf * msg;
	
// 	while ( 1) {
// 		if ( i!=0 && i==5 ) {
// 			msg = receive_message(NULL);
// 			#ifdef DEBUG_0
// 			printf("proc2 got msg: %s\n", msg->mtext);
// 			#endif /* DEBUG_0 */
// 		}
// 		
// 		if ( i!=0 && i==30 ) {
// 			msg = receive_message(NULL);
// 			#ifdef DEBUG_0
// 			printf("proc2 got msg: %s\n", msg->mtext);
// 			#endif /* DEBUG_0 */
// 		}
// 		
// 		if ( i != 0 && i%5 == 0 ) {
// 			uart0_put_string("\n\r");
// 			counter++;
// 			if ( counter == 8 ) {
// 				//ret_val = set_process_priority(PID_P1, HIGH);
// 				break;
// 			} else {
// 				ret_val = release_processor();
// 			}
// #ifdef DEBUG_0
// 			printf("proc2: ret_val=%d\n", ret_val);
// #endif /* DEBUG_0 */
// 		}
// 		uart0_put_char('0' + i%10);
// 		i++;
// 	}
// 	c = "proc2 end of testing\n\r";
// 	uart0_put_string(c);
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

