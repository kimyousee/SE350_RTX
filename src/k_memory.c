/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */

#include "k_memory.h"
#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; /* The last allocated stack low address. 8 bytes aligned */
               /* The first stack starts at the RAM high address */
	       /* stack grows down. Fully decremental stack */
U32 *free_block; //points to available space
U32 *end_block;  //points to end of the linked list
struct mem_blk *head;
struct mem_blk *tail;
typedef struct mem_blk {
	struct mem_blk *next_blk;
};
//PriorityQueue blocked_resource_q;

/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- gp_stack
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|<--- p_end
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|
          |        PCB pointers       |
          |---------------------------|<--- gp_pcbs
          |        Padding            |
          |---------------------------|  
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|          
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

*/

void memory_init(void)
{
	U8 *p_end = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit;
	int i;
	struct mem_blk b1;
  
	/* 4 bytes padding */
	p_end += 4;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)p_end;
	p_end += NUM_TEST_PROCS * sizeof(PCB *);
  
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
#ifdef DEBUG_0  
	printf("gp_pcbs[0] = 0x%x \n", gp_pcbs[0]);
	printf("gp_pcbs[1] = 0x%x \n", gp_pcbs[1]);
#endif
	
	/* prepare for alloc_stack() to allocate memory for stacks */
	
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
  
	/* allocate memory for heap, not implemented yet*/

	
	head = (struct mem_blk *)(gp_stack-8);
	tail = (struct mem_blk *)(gp_stack-8);
	printf("%x\n", tail);
	for ( i = 0; i < 30; i++ ) {
		tail->next_blk = (struct mem_blk *)(tail-150);
		printf("%x\n", tail->next_blk);
		tail = tail->next_blk;
		//printf("%x\n", tail);
		//printf("%x\n", head);
	}
	tail->next_blk = NULL;
	printf("%x\n", head->next_blk);
	printf("%x\n", head->next_blk->next_blk);
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);
	
	/* 8 bytes alignement adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}


void *k_request_memory_block(void) {
	struct mem_blk *free_mem = head;
	
	//printf("%x\n", gp_current_process);
	printf("%x\n", head->next_blk);
#ifdef DEBUG_0 
	printf("k_request_memory_block: entering...\n");
#endif /* ! DEBUG_0 */
	
	while(head == NULL){
		//blocked_resource_q.push(gp_current_process);
		//gp_current_process.setState(PROC_STATE_E.BLK);
		//k_release_processor();
	}
	printf("%x\n", head);
	head = head->next_blk;
	printf("%x\n", head);
	printf("%x\n", (void *) (free_mem));
	return (void *) (free_mem);
}

int k_release_memory_block(void *p_mem_blk) {
#ifdef DEBUG_0 
	printf("k_release_memory_block: releasing block @ 0x%x\n", p_mem_blk);
#endif /* ! DEBUG_0 */
	
	if (p_mem_blk == NULL) {
		return RTX_ERR;
	}
	
	//if (!blocked_resource_q.isEmpty()) {
		//PCB *process = blocked_resource_q.pop();
		//process.setMemory(p_mem_blk);
		//process.setState(PROC_STATE_E.RDY);
	//} else {
		tail->next_blk = p_mem_blk;
		((struct mem_blk*)p_mem_blk)->next_blk = NULL;
		tail = p_mem_blk;
	//}
	
	return RTX_OK;
}
