/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/02/28
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "real_user_proc.h"
#include "string.h"
#include <stdlib.h>

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_user_procs[NUM_USER_PROCS];

void set_user_procs() {
	int i;
	for( i = 0; i < NUM_USER_PROCS; i++ ) {
		g_user_procs[i].m_stack_size=USR_SZ_STACK;
	}
  
	g_user_procs[0].m_pid = (U32)PID_CLOCK;
	g_user_procs[0].mpf_start_pc = &wall_clock_proc;
	g_user_procs[0].m_priority = MEDIUM;
	
	g_user_procs[1].m_pid = (U32)PID_SET_PRIO;
	g_user_procs[1].mpf_start_pc = &set_priority_proc;
	g_user_procs[1].m_priority = MEDIUM;
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
	t[8] = '\n';
	t[9] = '\r';
	t[10] = '\0';
}

int charToTime(char *c, int s) {
	int h, m, e;
	
	h = (c[s]-'0') * 10 + (c[s+1]-'0');
	m = (c[s+3]-'0') * 10 + (c[s+4]-'0');
	e = (c[s+6]-'0') * 10 + (c[s+7]-'0');
	return h*3600 + m*60 + e;
}

int setTime(int newTime) {
	return newTime % 86400;
}

void wall_clock_display(int time) {
	MSG_BUF *msg;
	msg = (MSG_BUF *) request_memory_block();
	msg->mtype = UPDATE_TIME;
	delayed_send(PID_CLOCK, msg, ONE_SECOND);
	
	msg = (MSG_BUF *) request_memory_block();
	msg->mtype = CRT_DISPLAY;
	timeToChar(time, msg->mtext);
	
	send_message(PID_CRT, msg);
}

int checkInt(char *token) {
	int i = 0;
	char c = token[0];
	while (c!='\0') {
		if (!(c >= '0' && c <= '9') && !(i == 0 && c=='-')) {
			return 0;
		}
		i++;
		c = token[i];
	}
	return 1;
}

void wall_clock_proc() {
	MSG_BUF *msg;
	int i = 0;
	MSG_BUF *p_msg_env;
	long time = 0; // second
	int start = 0;

	while (1) {
		if (i==0) {
			i++;
			p_msg_env = (struct msgbuf *) request_memory_block();
			p_msg_env->mtype = KCD_REG;
			p_msg_env->mtext[0] = '%';
			p_msg_env->mtext[1] = 'W';
			p_msg_env->mtext[2] = '\0';
			send_message(PID_KCD, (void *)p_msg_env);
		}
		msg = receive_message((int*)PID_CLOCK);
		switch (msg->mtype) {
			case UPDATE_TIME:
				if (start == 1) {
					time = setTime(time+1);
					wall_clock_display(time);
				}
				break;
			default:
				switch (msg->mtext[2]) {
					case 'R':
						time = 0;
						break;
					case 'S':
						time = setTime(charToTime(msg->mtext, 4));
						if (start == 0) {
							start = 1;
							wall_clock_display(time);
						}
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

void send_error_msg(char *err) {
	MSG_BUF *msg;
	msg = (MSG_BUF *) request_memory_block();
	msg->mtype = CRT_DISPLAY;
	strcpy(msg->mtext, err);
	
	send_message(PID_CRT, msg);
}

void set_priority_proc(void) {
	MSG_BUF *msg;
	int i = 0;
	MSG_BUF *p_msg_env;
	char *token;
	int id=-1; 
	int newPri=-1;
	int index = 0;
	int errFlag = 0;
	
	while (1) {
		if (i==0) {
			i++;
			p_msg_env = (struct msgbuf *) request_memory_block();
			p_msg_env->mtype = KCD_REG;
			p_msg_env->mtext[0] = '%';
			p_msg_env->mtext[1] = 'C';
			p_msg_env->mtext[2] = '\0';
			send_message(PID_KCD, (void *)p_msg_env);
		}
		msg = receive_message((int*)PID_SET_PRIO);
		
 		if (msg->mtext[0] == '%' && msg->mtext[1] == 'C' && msg->mtext[2] == ' ') {
			token = strtok(msg->mtext, " ");
			token = strtok(NULL, " ");
			while(token) {
        //printf("%s", token);
				if (index > 1) {
					//ERR
					errFlag = 1;
					#ifdef DEBUG_0
					printf("Check priority error \n\r");
					#endif
					break;
				}
				if (checkInt(token)) {
					if (index == 0) {
						id = atoi(token);
					} else if (index == 1) {
						newPri = atoi(token);
					}
				} else {
					//ERR
					errFlag = 1;
					#ifdef DEBUG_0
					printf("Check priority error \n\r");
					#endif
					break;
				}
        token = strtok(NULL, " ");
				index ++;
			}
		}
			index = 0;
			if (errFlag) {
				send_error_msg("Invalid input for set priority command\n\r");
			} else {
				int retval = set_process_priority(id, newPri);
				if (retval == RTX_ERR) {
					send_error_msg("Invalid input for set priority command\n\r");
				}
			}
			errFlag = 0;
			//printf("id=%d, pri=%d\n\r", id, newPri);
// 			int i = 3;
// 			char c =  msg->mtext[i];
// 			int len = strlen(msg->mtext);
// 			/*for (i=3; i<len; i++) {
// 				%C 12 56 
// 			}*/
// 			while (c != ' ' && c != '\0') {
// 				if (c - '0' < 0 || c - '0' > 9) {
// 				}
// 				
// 				i++;
// 				c =  msg->mtext[i];
// 				if (c == '\0') {
// 					break;
// 				}
// 			}
// 			
 		}
	}
