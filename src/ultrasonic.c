#include "ultrasonic.h"
#include "LEDs.h"
#include "delay.h"
#include <stdio.h>
#include "lcd_4bit.h"
volatile int measureFlag = 0;
volatile uint16_t ticksElapsed = 0;
volatile int overflow = 0;
volatile float currMeasurement = 0;
static int echoFallingEdge = 0;

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
	//Enable_TPM();
	PIN_TRIG_PT->PSOR |= PIN_TRIG;
	//Delay(10);
	//PIN_TRIG_PT->PCOR |= PIN_TRIG;
	Start_PIT();
}

int Measure_Reading(float* measurement) {
	
	//Speed of sound in cm/ms
	float speedOfSound = 34.3;
	float timeElapsed = 0;
	
	while(!measureFlag) {
		if(overflow == 1){
			
			//Failure
			return 0;
		}
	}
	//convert ticks to miliseconds
	timeElapsed = ticksElapsed * TPM_PLL_Clock_Speed(1) * 1000;
	
	//Distance (cm) = (Time of Entire Pulse (ms) / 2) * speed of sound (cm/ms)
	*measurement = (timeElapsed / 2) * speedOfSound;
	
	//reset ticks elapsed
	ticksElapsed = 0;
	
	//Success
	return 1;
}


void PORTA_IRQHandler(void){

	//clear pending IRQ
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	if(PORTA->ISFR & PIN_ECHO){
		
		// clear status flag for echo pin
		PORTA->ISFR &= PIN_ECHO;
		Control_RGB_LEDs(0,1,0); //test if there is output from echo
		
		
		if(!echoFallingEdge)
		{
			//start timer
			Start_PIT2();
			echoFallingEdge = 1;
		}
		
		//Falling edge, update the LCD
		else {
			if (overflow == 0)
			{
				ticksElapsed = PWM_MAX_COUNT - PIT_CVAL1;
				measureFlag = 1;
	//			sprintf(buffer,"%3.2f", currMeasurement);
	//			Set_Cursor(0,1);
	//			Print_LCD(buffer);
			}
				Stop_PIT2();
				echoFallingEdge = 0;
				overflow = 0;
		}

	}
}
