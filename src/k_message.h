#ifndef K_MESSAGE_H_
#define K_MESSAGE_H_
#include "k_rtx.h"
#include "priority_queue.h"

extern PCB *gp_current_process;
extern PCB **gp_pcbs;
extern PCB *blocked_receive_head;
extern PCB *blocked_receive_tail;
extern PQ *ready_queue;

void enqueue_block_receive(PCB *pcb);
void dequeue_block_receive(U32 pid);

void enqueue_msg(PCB *p, MSG_BUF *msg);
MSG_BUF* dequeue_msg(PCB *p);

MSG_BUF *receive_message_nonblocking(PCB *p);
void k_send_message(int receiving_pid, MSG_BUF *msg);
MSG_BUF *k_receive_message(int *p_pid);

#endif
