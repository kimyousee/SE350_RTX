#include "stdlib.h"
#include "rtx.h"
#include "uart_polling.h"
#include "system_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_system_procs[NUM_SYSTEM_PROCS];

void set_system_procs() {
	int i;
	for( i = 0; i < NUM_SYSTEM_PROCS; i++ ) {
		g_system_procs[i].m_stack_size=0x100;
	}
  
	g_system_procs[0].m_pid = PID_KCD;
	g_system_procs[0].mpf_start_pc = &KCD_proc;
	g_system_procs[0].m_priority   = LOW;
	
	g_system_procs[1].m_pid = PID_CRT;
	g_system_procs[1].mpf_start_pc = &CRT_proc;
	g_system_procs[1].m_priority   = LOW;
}

void KCD_proc(void) {
	void *msg;
	//array -> {keyword, proc};
	while(1) {
		//receive_message();
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
				//if msg->body == keyword:
					//forwardmessage to proc;
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
