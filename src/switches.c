#include <MKL25Z4.H>
#include "switches.h"
#include "LEDs.h"

void init_switch(void) {
	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */ //system clock gating control register 5

	/* Select GPIO and enable pull-up resistors and interrupts 
		on falling edges for pins connected to switches */
	//(pin control register)
	PORTD->PCR[SW_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a); //Set to GPIO, set pull up resistor, enable pull up resistor, falling edge
	
	/* Set port D switch bit to inputs */
	PTD->PDDR &= ~MASK(SW_POS); //change to input,PDDR(port data direction register)

	/* Enable Interrupts */
	NVIC_SetPriority(PORTD_IRQn, 128); // 0, 64, 128 or 192, second lowest priority
	NVIC_ClearPendingIRQ(PORTD_IRQn);  //go to icpr and clear pending flags
	NVIC_EnableIRQ(PORTD_IRQn); //enable the isr by using the iser for portD 31
}

void PORTD_IRQHandler(void) {  
	Control_RGB_LEDs(1,1,1);	
	NVIC_ClearPendingIRQ(PORTD_IRQn); //go to icpr and clear the pending flag for portD
	if ((PORTD->ISFR & MASK(SW_POS))) { // checks the interrupt status flag register for port d at the switch position to see which interrupt
		Control_RGB_LEDs(1,1,0);
	}
	// clear status flags 
	PORTD->ISFR = 0xffffffff; //write 1 to clear the flags of ISFR
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
