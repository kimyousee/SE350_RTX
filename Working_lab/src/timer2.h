/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2013/02/12
 */
#ifndef _TIMER2_H_
#define _TIMER2_H_

#include <LPC17xx.h>

extern uint32_t timer2_init ( uint8_t n_timer );  /* initialize timer n_timer */
extern LPC_TIM_TypeDef *pTimer2;

void start_new_timer(int curr_process);

// Use this after context switching. Only pauses when current process is not the one we are timing
void pause_timer(int curr_process);

// only starts when curr_process is what we are timing
void start_timer(int curr_process);

int end_timer();

#endif /* ! _TIMER2_H_ */
