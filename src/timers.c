#include "timers.h"
#include "ultrasonic.h"
#include "LEDs.h"
#include <stdio.h>

extern int overflow;
extern int measureFlag;

void Init_PIT(unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	//PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(PWM_MAX_COUNT);
	
	// No chaining
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
}

void Start_PIT2(void) {
// Enable counter
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Start_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void Stop_PIT2(void) {
// Disable counter
	PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;
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
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
		
		// Do ISR work
		overflow = 1;
		measureFlag = 1;
		Control_RGB_LEDs(1,0,0);
	}	
}

void Init_TPM()
{
	//turn on clock to TPM 
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	//set clock source for tpm
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

	//load the counter and mod
	TPM0->MOD = PWM_MAX_COUNT;
		
	//set channel 4 to input capture mode on both edges and enable channel interrupts
	TPM0->CONTROLS[4].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_MSA_MASK;

	//Enable Timer Interrupt with clock divider as 8
	TPM0->SC = TPM_SC_CMOD(1) | TPM_SC_PS(3);
	
	//avoid any interrupt trigger after intialization
	Disable_TPM();
}

void Init_TPM_Interrupt(){
	//Initalize interrupts for TPM
	NVIC_SetPriority(TPM0_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM0_IRQn); 
	NVIC_EnableIRQ(TPM0_IRQn);
}

void Enable_TPM(){
	// Ensure the clock is disabled
	TPM0->SC &= ~TPM_SC_CMOD(3);
	
	//Enable Clock
	TPM0->SC |= TPM_SC_CMOD(1);
}

void Disable_TPM(){
	TPM0->SC &= ~TPM_SC_CMOD(3);
}

float Clock_Speed(int prescaleMode){
	
	//return clock speed in seconds/tick
	float val = PLL_CLOCK_FREQUENCY / prescaleMode;
	val = 1 / val;
	return val;
}

// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   