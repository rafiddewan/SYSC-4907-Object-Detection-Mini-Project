#include "timers.h"
#include "MKL25Z4.h"
#include "ultrasonic.h"
#include <stdio.h>

extern volatile int measureFlag;
extern volatile int overflow;
extern volatile int timeElapsed;
static int echoFallingEdge = 0;

void Init_PIT(unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
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
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		// Do ISR work
		PIN_TRIG_PT->PCOR |= PIN_TRIG;
		Stop_PIT();
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
		
	//set channel 4 to input capture mode
	TPM0->CONTROLS[3].CnSC = TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK;

	//Enable Timer Interrupt
	TPM0->SC = (TPM_SC_TOIE_MASK | TPM_SC_CMOD(1) | TPM_SC_PS(3));
	
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
	TPM0->SC &= ~TPM_SC_CMOD(3);
	TPM0->SC |= TPM_SC_CMOD(3);
}

void Disable_TPM(){
	TPM0->SC &= ~TPM_SC_CMOD(3);
}

float TPM_PLL_Clock_Speed(int prescaleMode){
	//return clock speed in seconds/tick
	float val = PLL_CLOCK_FREQUENCY / prescaleMode;
	val = 1 / val;
	return val;
}

void TPM0_IRQHandler(void) {
	
	//Keep track of overflow for time elapsed
	if(TPM0_SC & TPM_SC_TOF_SHIFT){
		
		//clear timer overflow flag and increment overflow counter
		TPM0_SC |= TPM_SC_TOF_MASK;
		overflow++;
	}
	
	//Rising edge or falling edge has occured, measurement has either started or completed
	if(TPM0_C4SC & TPM_CnSC_CHF_MASK) {
		
		//Clear channel flag
		TPM0_C4SC |= TPM_CnSC_CHF_MASK;
		
		//When it's rising edge start measuring
		if(!echoFallingEdge) {
			
			//Clear the count on the tpm to start capture for the input
			TPM0_CNT = 0;
			
			//Signal to start measurement
			echoFallingEdge = 1;
		}
		
		//When it's falling edge capture time elapsed
		else {
			// Get the time elapsed
			timeElapsed = TPM0_CNT + overflow*PWM_MAX_COUNT;
			
			//Set the flag that the measurement is complete
			measureFlag = 1;
		}
	}
}

// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   