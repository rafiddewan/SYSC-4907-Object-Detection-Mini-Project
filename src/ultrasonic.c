#include "ultrasonic.h"
#include "timers.h"
#include "LEDs.h"
#include "Delay.h"
#include <MKL25Z4.h>
#include "lcd_4bit.h"
#include <stdio.h>

volatile int echo_started = 0;
char buffer[11];
uint32_t pit_count = 0;

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
	PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] |= PORT_PCR_IRQC(0x0b);
	
	PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] &= ~PORT_PCR_PS_MASK;
	PIN_TRIG_PORT->PCR[PIN_TRIG_SHIFT] |= PORT_PCR_PE_MASK | PORT_PCR_PE_MASK;
	
	/* Enable Interrupts */
	NVIC_SetPriority(PORTA_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTA_IRQn); 
	NVIC_EnableIRQ(PORTA_IRQn);
}

void Generate_Trigger(){
	PIN_TRIG_PT->PSOR |= PIN_TRIG;
	//Delay(10);
	//PIN_TRIG_PT->PCOR |= PIN_TRIG;
	Start_PIT();
	//Control_RGB_LEDs(0,1,0);
}

void Clear_Trigger(){
	PIN_TRIG_PT->PCOR |= PIN_TRIG;
}

void PORTA_IRQHandler(void){
	//clear pending IRQ
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	if(PORTA->ISFR & PIN_ECHO){
		
		// clear status flag for timer channel 0
		PORTA->ISFR &= PIN_ECHO;
		Control_RGB_LEDs(0,1,0); //test if there is output from echo
		if(!echo_started)
		{
			//start timer
			Set_Cursor(0,0);
			Print_LCD("Echo Started");
			echo_started = 1;
		}
		else {
			Set_Cursor(0,1);
			Print_LCD("Echo Ended");
			Control_RGB_LEDs(1,1,0);
			echo_started = 0;
		}

	}
 }
