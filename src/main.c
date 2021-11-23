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
	float measurement = 0;
	Init_RGB_LEDs();
	Init_Ultrasonic();
	Init_PIT(BUS_CLOCK_FREQUENCY/240); //gives us a period of 10 microseconds
	//Init_LCD();
	__enable_irq();
	
	while(1){
		Generate_Trigger();
		Measuring_Reading(&measurement);
		
		//Reccomended delay for each trigger after reading is about 50ms
		Delay(250);
	}
	
	/*
	Clear_LCD();
  
	Set_Cursor(0,0);
	Print_LCD("Hello World!");
	*/
}
