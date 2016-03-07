/* @brief: common defines and structs for both kernel and user 
 * @file: common.h 
 * @author: Yiqing Huang
 * @date: 2016/02/24
 */

#ifndef COMMON_H_
#define COMMON_H_

/* Definitions */

#define BOOL unsigned char

#define TRUE 1
#define FALSE 0
#define NULL 0
#define RTX_ERR -1
#define RTX_OK 0

#define NULL 0
#define NUM_TEST_PROCS 6
#define NUM_SYSTEM_PROCS 2
#define NUM_I_PROCS 2
#define NUM_USER_PROCS 1
#define TOTAL_PROCS (NUM_TEST_PROCS+NUM_SYSTEM_PROCS+NUM_I_PROCS+NUM_USER_PROCS+1)

/* Process Priority. The bigger the number is, the lower the priority is*/
#define HIGH    1
#define MEDIUM  2
#define LOW     3
#define LOWEST  4

/* Process IDs */
#define PID_NULL 0
#define PID_P1   1
#define PID_P2   2
#define PID_P3   3
#define PID_P4   4
#define PID_P5   5
#define PID_P6   6
#define PID_A    7
#define PID_B    8
#define PID_C    9
#define PID_SET_PRIO     10
#define PID_CLOCK        11
#define PID_KCD          12
#define PID_CRT          13
#define PID_TIMER_IPROC  14
#define PID_UART_IPROC   15

/* Memory Constants */
#define MEMORY_BLOCKS 30
#define MEMORY_BLOCK_SIZE 64

/* Message Types */
#define DEFAULT 0
#define KCD_REG 1
#define CRT_DISPLAY 2
#define KEYBOARD_INPUT 3
#define UPDATE_TIME 4

/* process states, note we only assume three states in this example */
typedef enum {NEW = 0, RDY, RUN, BLK, BLK_RCV} PROC_STATE_E;  

#ifdef DEBUG_0
#define ONE_SECOND 1000
#else
#define ONE_SECOND 1000
#endif

#ifdef DEBUG_0
#define USR_SZ_STACK 0x500
#define SYS_SZ_STACK 0x500
#else
#define USR_SZ_STACK 0x500
#define SYS_SZ_STACK 0x500
#endif /* DEBUG_0 */


/* ----- Types ----- */
typedef unsigned char U8;
typedef unsigned int U32;

/* common data structures in both kernel and user spaces */

/* initialization table item, exposed to user space */
typedef struct proc_init
{	
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

/* message buffer */
typedef struct msgbuf
{
#ifdef K_MSG_ENV
	void *mp_next;		/* ptr to next message received*/
	int m_send_pid;		/* sender pid */
	int m_recv_pid;		/* receiver pid */
	int m_kdata[5];		/* extra 20B kernel data place holder */
#endif
	int mtype;              /* user defined message type */
	char mtext[1];          /* body of the message */
} MSG_BUF;

typedef struct pcb 
{ 
	struct pcb *next;  /* next pcb */  
	U32 *mp_sp;		/* stack pointer of the process */
	U32 m_pid;		/* process id */
	U32 m_priority;
	PROC_STATE_E m_state;   /* state of the process */     
	void *mem_pointer; /* pointer to the memory after it's released */
	MSG_BUF *head_msg;
	MSG_BUF *tail_msg;
} PCB;

#endif // COMMON_H_
