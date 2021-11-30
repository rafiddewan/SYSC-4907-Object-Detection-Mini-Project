/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.h>
#include <stdio.h>
#include "timers.h"
#include "ultrasonic.h"
#include "lcd_4bit.h"
#include "LEDs.h"
#include "delay.h"
#include "gpio_defs.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	//float measurement = 0;
	Init_RGB_LEDs();
	Init_PIT(240); //gives us a period of 10 microseconds
	Init_Ultrasonic();
	//Init_LCD();
	__enable_irq();
	PIN_TRIG_PT->PSOR |= PIN_TRIG;
	Start_PIT();
	while(1) {
		//Generate_Trigger();
	  //Measure_Reading(&measurement);
		//Delay(1000);
	}
	
	/*
	Clear_LCD();
  
	Set_Cursor(0,0);
	Print_LCD("Hello World!");
	*/
}
