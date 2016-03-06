#include "stdlib.h"
#include "rtx.h"
#include "uart_polling.h"
#include "system_proc.h"
#include "string.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

typedef struct keyIdPairs
{
	char *key;
	int id;
} KEYID;

typedef struct map
{
	int len;
	KEYID *pairs[1];
} MAP;

int checkPrefix(char *c1, char *c2) {
	// Check if c1 matches a prefix of c2
	int i = strlen(c1);
	int j = strlen(c2);
	// assert i <= j
	int s = 0;
	while (s < i) {
		if (c1[s] != c2[s]) {
			return 0;
		}
		s ++;
	}
	return 1;
}

void registerKeyword(MAP *map, char *keyword, int proc_id) {
	int length = map->len;
	void *mem = request_memory_block();
	KEYID *pair = (KEYID *)mem;
	pair->key = keyword;
	pair->id = proc_id;
	map->pairs[length] = pair;
	map->len ++;
}

/* initialization table item */
PROC_INIT g_system_procs[NUM_SYSTEM_PROCS];

void set_system_procs() {
	int i;
	for( i = 0; i < NUM_SYSTEM_PROCS; i++ ) {
		g_system_procs[i].m_stack_size=0x300;
	}
  
	g_system_procs[0].m_pid = PID_KCD;
	g_system_procs[0].mpf_start_pc = &KCD_proc;
	g_system_procs[0].m_priority   = HIGH-1;
	
	g_system_procs[1].m_pid = PID_CRT;
	g_system_procs[1].mpf_start_pc = &CRT_proc;
	g_system_procs[1].m_priority   = HIGH-1;
}

void KCD_proc(void) {
	MAP *map;
	char *keyword;
	int i;
	void *mem = request_memory_block();
	MSG_BUF *msg;
	MSG_BUF *sending_msg;
	int current_msg_length = 0;
	map = (MAP *)mem;
	map->len = 0;
	mem = request_memory_block();
	sending_msg = (MSG_BUF *)mem;
	while(1) {
		msg = (MSG_BUF*)receive_message((int *)PID_KCD);
		switch(msg->mtype){
			case KCD_REG:
				registerKeyword(map, msg->mtext, msg->m_send_pid);
				break;
			case KEYBOARD_INPUT:
				if (msg->mtext[0] != 0x0D) {
					sending_msg->mtext[current_msg_length] = msg->mtext[0];
					current_msg_length ++;
					msg->mtext[1] = '\0';
				} else {
					msg->mtext[1] = '\n';
					msg->mtext[2] = '\r';
					msg->mtext[3] = '\0';
					
					for (i=0; i<map->len; i++) {
						if (checkPrefix(map->pairs[i]->key, sending_msg->mtext)) {
							send_message(map->pairs[i]->id, sending_msg);
							#ifdef DEBUG_0
							printf("forwarding to registered process %d\n\r", map->pairs[i]->id);
							#endif
						}
					}
					
					mem = request_memory_block();
					sending_msg = (MSG_BUF *)mem;
					current_msg_length = 0;
				}
				msg->mtype = CRT_DISPLAY;
				send_message(PID_CRT, msg);
				break;
			default: break;
		}
	}
}

void CRT_proc(void) {
	PCB *pcb;
	MSG_BUF *msg;
	while(1) {
		msg = (MSG_BUF*)receive_message((int*)PID_CRT);
		switch (msg->mtype) {
			case CRT_DISPLAY:
				send_message(PID_UART_IPROC, msg);
				break;
			default: break;
		}		
	}
}
