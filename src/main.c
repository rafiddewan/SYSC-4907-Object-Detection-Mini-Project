/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.h>
#include <stdio.h>
#include <stdlib.h>
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
	char *measurementStr;
	int validReading = 0;
	Init_RGB_LEDs();
	Init_Ultrasonic();
	Init_PIT(240); //gives us a period of 10 microseconds
	Init_LCD();
	__enable_irq();
	Clear_LCD();
	
	measurementStr = (char*) malloc(17 * sizeof(char));
	while(1) {
		Generate_Trigger();
	  validReading = Measure_Reading(&measurement);
		if(validReading) {
			sprintf(measurementStr,"%3.2f", measurement);
		}
		else{
			measurementStr = "Undetected Obj";
		}
		Set_Cursor(0,1);
		Print_LCD(measurementStr);
		Delay(1000);
	}
	
	/*
	Clear_LCD();
  
	Set_Cursor(0,0);
	Print_LCD("Hello World!");
	*/
}
