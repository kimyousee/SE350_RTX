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
#include <stdlib.h>

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
  
	g_user_procs[0].m_pid = (U32)PID_CLOCK;
	g_user_procs[0].mpf_start_pc = &wall_clock_proc;
	g_user_procs[0].m_priority = MEDIUM;
}

void timeToChar(int time, char* t) {
	int h, h1, h2, m, m1, m2, s, s1, s2;
	h = time/3600;
	h2 = h % 10;
	h1 = (h - h2)/10;
	m = (time - (h*3600)) / 60;
	m2 = m % 10;
	m1 = (m - m2)/10;
	s = (time - (h*3600 + m*60));
	s2 = s % 10;
	s1 = (s - s2)/10;
	t[0] = h1+'0';
	t[1] = h2+'0';
	t[2] = ':';
	t[3] = m1+'0';
	t[4] = m2+'0';
	t[5] = ':';
	t[6] = s1+'0';
	t[7] = s2+'0';
}

int charToTime(char *c, int s) {
	int h, m, e;
	h = (c[s]-'0') * 10 + (c[s+1]-'0');
	m = (c[s+3]-'0') * 10 + (c[s+4]-'0');
	e = (c[s+6]-'0') * 10 + (c[s+7]-'0');
	return h*3600 + m*60 + e;
}

void wall_clock_display(int time) {
	MSG_BUF *msg;
	msg = (MSG_BUF *) request_memory_block();
	msg->mtype = UPDATE_TIME;
	delayed_send(PID_TIMER_IPROC, msg, 10);
	
	msg = (MSG_BUF *) request_memory_block();
	msg->mtype = CRT_DISPLAY;
	timeToChar(time, msg->mtext);
	
	uart0_put_string(msg->mtext);
	uart0_put_string("\n\r");
	//send_message(PID_CRT, msg);
}

void wall_clock_proc() {
	MSG_BUF *msg;
	void *mem;
	int i = 0;
	MSG_BUF *p_msg_env;
	long time = 0; // second
	char second[2], minute[2], hour[2];
	int start = 0;

	while (1) {
		if (i==0) {
			i++;
			p_msg_env = (struct msgbuf *) request_memory_block();
			p_msg_env->mtype = KCD_REG;
			p_msg_env->mtext[0] = '%';
			p_msg_env->mtext[1] = 'W';
			p_msg_env->mtext[2] = '\0';
			//send_message(PID_KCD, (void *)p_msg_env);
		}
		msg = receive_message((int*)PID_CLOCK);
		switch (msg->mtype) {
			case UPDATE_TIME:
				if (start == 1) {
					time ++;
					wall_clock_display(time);
				}
				break;
			default:
				switch (msg->mtext[2]) {
					case 'R':
						time = 0;
						break;
					case 'S':
						time = charToTime(msg->mtext, 4);
						start = 1;
					
						wall_clock_display(time);
						break;
					case 'T':
						start = 0;
						break;
					default: break;
				}
				break;
		}
		release_memory_block((void*)msg);
		release_processor();
	}
}
