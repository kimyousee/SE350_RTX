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
		g_system_procs[i].m_stack_size=0x100;
	}
  
	g_system_procs[0].m_pid = PID_KCD;
	g_system_procs[0].mpf_start_pc = &KCD_proc;
	g_system_procs[0].m_priority   = HIGH;
	
	g_system_procs[1].m_pid = PID_CRT;
	g_system_procs[1].mpf_start_pc = &CRT_proc;
	g_system_procs[1].m_priority   = LOW;
}

void KCD_proc(void) {
	void *msg;
	MAP *map;
	char *keyword;
	int i;
	void *mem = request_memory_block();
	map = (MAP *)mem;
	map->len = 0;
	//registerKeyword(map, "", -1);
	registerKeyword(map, "abc", 1);
	registerKeyword(map, "ab", 2);
	registerKeyword(map, "abc", 4);
	registerKeyword(map, "c", 3);
	//array -> {keyword, proc};
	while(1) {
		//receive_message();
		// Check for keywords
		
		//(void *)
		
		//printf("KCD Proc\n");
		//MSG_BUF* msg = (MSG_BUF*)k_receive_message(NULL);
		//switch(msg->mtype){
			//case KCD_REG:
				//keyword = msg->body;
				//proc = msg->sender;
				//array.add({keyword, proc});
// 				if (msg->mtext[0] == '%'){
// 					if (msg->mtext[1] == 'W'){
// 						k_send_message(PID_CLOCK, (void*)msg);
// 					}
// 				}
				//break;
			//case CRT_DISPLAY:
		keyword = "abcd";
		for (i=0; i<map->len; i++) {
			if (checkPrefix(map->pairs[i]->key, keyword)) {
				//forward message
				printf("%d\n\r", map->pairs[i]->id);
			}
		}
		//forward to CRT
				//if 
				//k_send_message(PID_CRT, (void *)msg);
				//break;
			//default: break;
		//}
		
		
		
		release_processor();
	}
}

void CRT_proc(void) {
	while(1) {
		//receive_message();
		//printf("CRT Proc\n");
		release_processor();;
	}
}
