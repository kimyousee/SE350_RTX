/**
 * @file:   usr_procA.c
 * @brief:  Three user processes: proc1...4 to set priority low check
 * @author: G031
 * @date:   2016/02/03
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 * Expected UART output:
 * 01234
 * ABCDE
 * 56789
 * FGHIJ01234
 * abcde
 * 56789
 * proc2 end of testing
 * lmnop
 * fghij
 * qrstu
 * abcde
 * lmnop
 * fghij
 * proc1 end of testing
 * qrstu
 * proc4 end of testing
 * 
 * ABCDE
 * FGHIJ
 * proc3 end of testing
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
		g_test_procs[i].m_stack_size=0x100;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[0].m_priority   = MEDIUM;
	
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[1].m_priority   = HIGH;
	
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[2].m_priority   = HIGH;
	
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[3].m_priority   = MEDIUM;

}

/**
 * @brief: a process that prints 2x5 lowercase letters
 */
void proc1(void)
{
	int i = 0;
	int j = 0;
	int counter = 0;
	int ret_val = 100;
	char *c;
	void *mem;
	void *mem2;
	int state;
	
	while ( 1 ) {
		
		if (i==5) {
			//for (j=0; j<MEMORY_BLOCKS; j++) {
			//	mem2 = mem;
			//	mem = request_memory_block();
			//}
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
	set_process_priority(1, LOWEST);
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
	
	while ( 1) {

		
		
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
	set_process_priority(2, LOWEST);
	while ( 1 ) {
		release_processor();
	}
}

void proc3(void)
{
	int i = 0;
	int ret_val = 20;
	int counter = 0;
	char* c;
	void *mem;
	
	while ( 1) {
		if (i == 10) {
			set_process_priority(3, LOW);
		}
		
		if ( i != 0 && i%5 == 0 ) {
 			uart0_put_string("\n\r");
			counter++;
			if ( counter == 4 ) {
				break;
			} else {
				ret_val = release_processor();
			}
#ifdef DEBUG_0
			printf("proc3: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
		}
		uart0_put_char('A' + i%10);
		i++;
	}
	c = "proc3 end of testing\n\r";
	uart0_put_string(c);
	set_process_priority(3, LOWEST);
	while ( 1 ) {
		release_processor();
	}
}

void proc4(void)
{
	int i = 0;
	int ret_val = 20;
	int counter = 0;
	char* c;
	void *mem;
	
	while ( 1) {
		
		if ( i != 0 && i%5 == 0 ) {
 			uart0_put_string("\n\r");
			counter++;
			if ( counter == 4 ) {
				break;
			} else {
				ret_val = release_processor();
			}
#ifdef DEBUG_0
			printf("proc4: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
		}
		uart0_put_char('l' + i%10);
		i++;
	}
	c = "proc4 end of testing\n\r";
	uart0_put_string(c);
	set_process_priority(4, LOWEST);
	while ( 1 ) {
		release_processor();
	}
}

