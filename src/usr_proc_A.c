/**
 * @file:   usr_procA.c
 * @brief:  Two user processes: proc1...2 to do context switching testing
 * @author: G031
 * @date:   2016/02/03
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 * Expected UART output:
 * abcde
 * 01234
 * fghij
 * 56789
 * 01234
 * 56789
 * proc1 end of testing
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

//char *result;
//char *expected = "abcde\n\r01234\nfghij\n\r56789\n\r01234\n\r56789\n\rproc1 end of testing\n\rproc2 end of testing\n\r";
//int counter = 0;
//int testResult = 1;

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_stack_size=0x100;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[0].m_priority   = MEDIUM;
	
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[1].m_priority   = MEDIUM;

}

// void check_test_procs(char *c, int length) {
// 	int i;
// 	for (i=0; i<length; i++) {
// 		if (expected[counter] != c[i]) {
// 			testResult = 0;
// 		}
// 		counter++;
// 	}
// }

/**
 * @brief: a process that prints 2x5 lowercase letters
 */
void proc1(void)
{
	int i = 0;
	int counter = 0;
	int ret_val = 100;
	char *c;
	while ( 1 ) {
		
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");
			//check_test_procs("\n\r", 2);
			counter++;
			if ( counter == 2 ) {
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
		//check_test_procs((char*)('a'+i%10), 1);
		//result = strcat(result, (char *)('a'+i%10));
		i++;
	}
	c = "proc1 end of testing\n\r";
	uart0_put_string(c);
	//check_test_procs(c, strlen(c));
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
	
	while ( 1) {
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");
			//check_test_procs("\n\r", 2);
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
		//check_test_procs((char*)('0'+i%10), 1);
		i++;
	}
	c = "proc2 end of testing\n\r";
	uart0_put_string(c);
	//check_test_procs(c, strlen(c));
	while ( 1 ) {
// 		uart0_put_string(result);
// 		uart0_put_string(expected);
// 		if (testResult == 1 && counter == strlen(expected)) {
// 			uart0_put_string("test1 OK\n\r");
// 		} else {
// 			uart0_put_string("test1 FAIL\n\r");
// 		}
		
		release_processor();
	}
}

