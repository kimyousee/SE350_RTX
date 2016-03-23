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

#endif /* ! _TIMER2_H_ */
