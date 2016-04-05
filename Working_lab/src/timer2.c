/**
 * @brief timer.c - Timer example code. Tiemr IRQ is invoked every 1ms
 * @author T. Reidemeister
 * @author Y. Huang
 * @author NXP Semiconductors
 * @date 2012/02/12
 */

#include <LPC17xx.h>
#include "timer2.h"
#ifdef DEBUG_0
#include "printf.h"
#endif

#define BIT(X) (1<<X)
extern void Timer_i_Proc(void);
extern void check_priority(void);

/**
 * @brief: initialize timer. Only timer 0 is supported
 */
 
 LPC_TIM_TypeDef *pTimer2;

int start_time;
 
uint32_t timer2_init(uint8_t n_timer) 
{
	start_time = 0;
	if (n_timer == 0) {
		/*
		Steps 1 & 2: system control configuration.
		Under CMSIS, system_LPC17xx.c does these two steps
		
		----------------------------------------------------- 
		Step 1: Power control configuration.
		        See table 46 pg63 in LPC17xx_UM
		-----------------------------------------------------
		Enable UART0 power, this is the default setting
		done in system_LPC17xx.c under CMSIS.
		Enclose the code for your refrence
		//LPC_SC->PCONP |= BIT(1);
	
		-----------------------------------------------------
		Step2: Select the clock source, 
		       default PCLK=CCLK/4 , where CCLK = 100MHZ.
		       See tables 40 & 42 on pg56-57 in LPC17xx_UM.
		-----------------------------------------------------
		Check the PLL0 configuration to see how XTAL=12.0MHZ 
		gets to CCLK=100MHZ in system_LPC17xx.c file.
		PCLK = CCLK/4, default setting in system_LPC17xx.c.
		Enclose the code for your reference
		//LPC_SC->PCLKSEL0 &= ~(BIT(3)|BIT(2));	

		-----------------------------------------------------
		Step 3: Pin Ctrl Block configuration. 
		        Optional, not used in this example
		        See Table 82 on pg110 in LPC17xx_UM 
		-----------------------------------------------------
		*/
		pTimer2 = (LPC_TIM_TypeDef *) LPC_TIM1;

	} else { /* other timer not supported yet */
		return 1;
	}

	/*
	-----------------------------------------------------
	Step 4: Interrupts configuration
	-----------------------------------------------------
	*/

	/* Step 4.1: Prescale Register PR setting 
	   CCLK = 100 MHZ, PCLK = CCLK/4 = 25 MHZ
	   2*(12 + 1)*(1/25) * 10^(-6) s = 1.04*10^(-6) s = 1 microsecond
	   TC (Timer Counter) toggles b/w 0 and 1 every 12500 PCLKs
	   see MR setting below 
	*/
	pTimer2->PR = 12;  

	/* Step 4.2: MR setting, see section 21.6.7 on pg496 of LPC17xx_UM. */
	//pTimer2->MR0 = 1000;

	/* Step 4.3: MCR setting, see table 429 on pg496 of LPC17xx_UM.
	   Interrupt on MR0: when MR0 mathches the value in the TC, 
	                     generate an interrupt.
	   Reset on MR0: Reset TC if MR0 mathches it.
	*/
	//pTimer2->MCR = BIT(0) | BIT(1);

	/* Step 4.4: CSMSIS enable timer0 IRQ */
	NVIC_EnableIRQ(TIMER0_IRQn);

	/* Step 4.5: Enable the TCR. See table 427 on pg494 of LPC17xx_UM. */
	pTimer2->TCR = 1;
	return 0;
}

void start_timer(){
	start_time = pTimer2->TC;
}

int end_timer(){
	int time = pTimer2->TC - start_time;
	return time;
}