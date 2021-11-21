#include "ultrasonic.h"
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
    
    //pull up resistors
    PIN_ECHO_PORT->PCR[PIN_ECHO_SHIFT] |= PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0b);


}

void Generate_Trigger(){
    //generate a 10 microsecond pulse

}

void PortA_IRQHandler(void){
    //clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
    if(PORTA->ISFR & PIN_ECHO){
        if(!timer_started){
            //start timer
            Start_PIT();
            timer_started = 1;
        }
        else{
            //stop timer
            Stop_PIT();
            timer_started = 0;
        }

    }
}