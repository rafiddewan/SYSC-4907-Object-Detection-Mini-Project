/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.h>
#include <stdio.h>
#include "timers.h"
#include "ultrasonic.h"
#include "lcd_4bit.h"
#include "LEDs.h"
#include "delay.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	
	Init_RGB_LEDs();
	Init_PIT(BUS_CLOCK_FREQUENCY/240); //gives us a period of 10 microseconds
	//Init_LCD();
	__enable_irq();
	Generate_Trigger();
	
	/*
	Clear_LCD();
  
	Set_Cursor(0,0);
	Print_LCD("Hello World!");
	*/
}
