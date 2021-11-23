#include "ultrasonic.h"
#include "timers.h"
#include "LEDs.h"
#include <stdio.h>

volatile int measureFlag = 0;
volatile int ticksElapsed = 0;
volatile int overflow = 0;

void Init_Ultrasonic(void){
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

  //set to GPIO
	PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] &= ~PORT_PCR_MUX_MASK;          
	PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] |= PORT_PCR_MUX(1); 

	//PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] &= ~PORT_PCR_MUX_MASK;          
	//PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] |= PORT_PCR_MUX(1); 

	PIN_TRIG_PT->PDDR |= PIN_TRIG; //output
	
	//Set pin multiplexer to TPM0_CH4 mode and enable 
	PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] = PORT_PCR_MUX(3) | PORT_PCR_PE_MASK;
	
	//Initialize TPM and it's interrupts
	Init_TPM();
	Init_TPM_Interrupt();
	
	//PIN_ECHO_PT->PDDR &= ~PIN_ECHO; // input

	//pull down resistors
	PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] &= ~PORT_PCR_PS_MASK;
	//PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] |= PORT_PCR_IRQC(0x0b); Enable interrupt on pin as gpio
	
	PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] &= ~PORT_PCR_PS_MASK;
	
//	/* Enable Interrupts */
//	NVIC_SetPriority(PORTA_IRQn, 128); // 0, 64, 128 or 192
//	NVIC_ClearPendingIRQ(PORTA_IRQn); 
//	NVIC_EnableIRQ(PORTA_IRQn);
}

void Generate_Trigger(){
	PIN_TRIG_PT->PSOR |= PIN_TRIG;
	Start_PIT();
	Enable_TPM();
}

void Measure_Reading(float* measurement) {
	
	//Speed of sound in cm/ms
	float speedOfSound = 0.034;
	//Poll until we receive the measuring flag
	while(!measureFlag){
	}
		
	//disable clock in case of any other interrupts appearing after taking the measurements
	Disable_TPM();
	//reset overflow counter
	overflow = 0;
	
	//convert ticks to miliseconds
	float timeElapsed = ticksElapsed * TPM_PLL_Clock_Speed(8) * 1000;
	
	//Distance (cm) = (Time of Entire Pulse (ms) / 2) * speed of sound (cm/ms)
	*measurement = (timeElapsed / 2) * speedOfSound;
	
	//reset ticks elapsed
	ticksElapsed = 0;
}

//void PORTA_IRQHandler(void){
//	printf("Handler has been reached");
//	//clear pending IRQ
//	NVIC_ClearPendingIRQ(PORTA_IRQn);
//	if(PORTA->ISFR & PIN_ECHO){
//		// clear status flag for timer channel 0
//		PORTA->ISFR &= PIN_ECHO;
//		DEBUG_PORT->PCOR = MASK(DBG_ISR_POS);
//		Control_RGB_LEDs(1,0,0); //test if there is output from echo
//		/*if(!timer_started)
//		{
//			//start timer
//			Start_PIT();
//			timer_started = 1;
//		}
//		else
//		{
//			//stop timer
//			Stop_PIT();
//			timer_started = 0;
//		}*/

//	}
//}
