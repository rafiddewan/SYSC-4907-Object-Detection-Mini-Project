#include "ultrasonic.h"
#include "LEDs.h"
#include "delay.h"
#include <stdio.h>
#include "lcd_4bit.h"
volatile int measureFlag = 0;
volatile uint16_t ticksElapsed = 0;
volatile int overflow = 0;
volatile int echo_started = 0;
//static int echoFallingEdge = 0;
uint16_t temp = 0;
char buffer[9];

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
	
	Init_TPM();
}

void Generate_Trigger(){
	Enable_TPM();
	PIN_TRIG_PT->PSOR |= PIN_TRIG;
	//Delay(10);
	//PIN_TRIG_PT->PCOR |= PIN_TRIG;
	Start_PIT();
}

void Measure_Reading(float* measurement) {
	
	//Speed of sound in cm/ms
	float speedOfSound = 34.3;
	float timeElapsed = 0;
	//Poll until we receive the measuring flag
	while(!measureFlag){
		//printf("%lu", (unsigned long)TPM0_C3V);
	}
	
	Control_RGB_LEDs(1, 0, 0);
	
	//reset overflow counter
	overflow = 0;
	//convert ticks to miliseconds
	timeElapsed = ticksElapsed * TPM_PLL_Clock_Speed(8) * 1000;
	
	//Distance (cm) = (Time of Entire Pulse (ms) / 2) * speed of sound (cm/ms)
	*measurement = (timeElapsed / 2) * speedOfSound;
	
	//reset ticks elapsed
	ticksElapsed = 0;
}

//void TPM0_IRQHandler(void) {
//	
//	Control_RGB_LEDs(1, 1, 0);
//	//Keep track of overflow for time elapsed
//	if(TPM0_STATUS & TPM_STATUS_TOF_MASK){
//		
//		//clear timer overflow flag and increment overflow counter
//		TPM0_SC |= TPM_SC_TOF_MASK;
//		overflow++;
//	}
//	
//	//Rising edge or falling edge has occured, measurement has either started or completed
//	if(TPM0_STATUS & TPM_STATUS_CH4F_MASK) {
//		
//		//Clear channel flag
//		TPM0_C4SC |= TPM_CnSC_CHF_MASK;
//		
//		//When it's rising edge start measuring
//		if(!echoFallingEdge) {
//						
//			//Signal to start measurement
//			echoFallingEdge = 1;
//		}
//		
//		//When it's falling edge capture time elapsed
//		else {
//			// Get the time elapsed
//			ticksElapsed = TPM0_CNT + overflow*PWM_MAX_COUNT;
//			
//			//Set the flag that the measurement is complete
//			measureFlag = 1;
//			
//			//Disable TPM
//			Disable_TPM();
//		}
//	}
//}

void PORTA_IRQHandler(void){

	//clear pending IRQ
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	if(PORTA->ISFR & PIN_ECHO){
		
		// clear status flag for timer channel 0
		PORTA->ISFR &= PIN_ECHO;
		Control_RGB_LEDs(0,1,0); //test if there is output from echo
		
		if(TPM0->STATUS & TPM_STATUS_TOF_MASK){
			TPM0->STATUS |= TPM_STATUS_TOF_MASK;
			overflow = 1;
		}
		if(!echo_started)
		{
			//start timer
//			Set_Cursor(0,0);
//			Print_LCD("Echo Started");
			echo_started = 1;
			TPM0_CNT = 0;
			Enable_TPM();
		}
		else {
//			Set_Cursor(0,1);
//			Print_LCD("Echo Ended");
			temp = TPM0_CNT;
			Disable_TPM();
			ticksElapsed = temp + overflow*PWM_MAX_COUNT;
			Control_RGB_LEDs(1,1,0);
			echo_started = 0;
			overflow = 0;
			sprintf(buffer,"%d",ticksElapsed);
			Set_Cursor(0,1);
			Print_LCD(buffer);
		}

	}
}
