#include "timers.h"
#include "ultrasonic.h"
#include "LEDs.h"
#include <stdio.h>

void Init_PIT(unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	//PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);

	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
}


void Start_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Stop_PIT(void) {
// Disable counter
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}

void PIT_IRQHandler() {
	
	//clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
		
		// Do ISR work
		PIN_TRIG_PT->PCOR |= PIN_TRIG;
		//Control_RGB_LEDs(1,0,0);
		Stop_PIT();
	}	
}

void Init_TPM()
{
	//turn on clock to TPM 
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	//set clock source for tpm
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK;
	
	//load the counter and mod
	TPM2->MOD = PWM_MAX_COUNT;
		
	//set channel 0 to input capture mode on both edges and enable channel interrupts
	TPM2->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK;
	//TPM2->CONTROLS[0].CnSC &= ~(TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
	
	TPM2->SC = (TPM_SC_TOIE_MASK | TPM_SC_CMOD(1) | TPM_SC_PS(6));
	
	//avoid any interrupt trigger after intialization
	//Disable_TPM();
	TPM2->CONTROLS[0].CnV = 0;
	Init_TPM_Interrupt();
}

void Init_TPM_Interrupt(){
	//Initalize interrupts for TPM
	NVIC_SetPriority(TPM2_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM2_IRQn); 
	NVIC_EnableIRQ(TPM2_IRQn);
}

void Enable_TPM(){
	// Ensure the clock is disabled
	TPM2->SC &= ~TPM_SC_CMOD(3);
	
	//Start reload counter value on trigger
	TPM2->CONF |= TPM_CONF_CROT_MASK;
	
	//Start counting on rising edge
	TPM2->CONF |= TPM_CONF_CSOT_MASK;
	
	//Allow timers to continue in debug mode
	TPM2->CONF |= TPM_CONF_DBGMODE(3);
	
	//set the external trigger to pin when the echo pulse intiates a trigger on it
	TPM2->CONF |= TPM_CONF_TRGSEL(0x0);
	
	//Enable Clock
	TPM2->SC |= TPM_SC_CMOD(1);
}

void Disable_TPM(){
	TPM2->SC &= ~TPM_SC_CMOD(3);
}

float TPM_PLL_Clock_Speed(int prescaleMode){
	//return clock speed in seconds/tick
	float val = PLL_CLOCK_FREQUENCY / prescaleMode;
	val = 1 / val;
	return val;
}

// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   