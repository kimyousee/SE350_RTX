/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/02/28
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "real_user_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_user_procs[NUM_USER_PROCS];

void set_user_procs() {
	int i;
	for( i = 0; i < NUM_USER_PROCS; i++ ) {
		g_user_procs[i].m_stack_size=0x100;
	}
  
	g_user_procs[0].m_pid = (U32)(PID_CLOCK);
	g_user_procs[0].mpf_start_pc = &wall_clock_proc;
	g_user_procs[0].m_priority = LOWEST;
}


void wall_clock_proc() {
	while (1) {
		printf("wall clock\n\r");
		release_processor();
	}
}
