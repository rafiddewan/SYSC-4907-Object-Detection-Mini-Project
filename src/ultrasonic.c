#include "ultrasonic.h"
#include "timers.h"
#include "LEDs.h"

volatile int timer_started = 0;

void Init_Ultrasonic(void){
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    //set to GPIO
	PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] &= ~PORT_PCR_MUX_MASK;          
	PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] |= PORT_PCR_MUX(1); 

	PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] &= ~PORT_PCR_MUX_MASK;          
	PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] |= PORT_PCR_MUX(1); 

	PIN_TRIG_PT->PDDR |= PIN_TRIG; //output
	PIN_ECHO_PT->PDDR &= ~PIN_ECHO; // input

	//pull down resistors and enable on rising and falling
	PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] &= ~PORT_PCR_PS_MASK;
	PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] |= PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0b);
	
	//PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] &= ~PORT_PCR_PS_MASK;
	
	/* Enable Interrupts */
	NVIC_SetPriority(PORTA_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTA_IRQn); 
	NVIC_EnableIRQ(PORTA_IRQn);
}

void Generate_Trigger(){
	PIN_TRIG_PT->PSOR |= PIN_TRIG;
	Start_PIT();
}

void PortA_IRQHandler(void){
	//clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
	if(PORTA->ISFR & PIN_ECHO){
		
		// clear status flag for timer channel 0
		PORTA->ISFR &= PIN_ECHO;
		
		Control_RGB_LEDs(1,0,0); //test if there is output from echo
		/*if(!timer_started)
		{
			//start timer
			Start_PIT();
			timer_started = 1;
		}
		else
		{
			//stop timer
			Stop_PIT();
			timer_started = 0;
		}*/

	}
}
