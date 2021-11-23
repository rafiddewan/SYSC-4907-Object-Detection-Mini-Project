/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.h>
#include <stdio.h>
#include "timers.h"
#include "ultrasonic.h"
#include "lcd_4bit.h"
#include "LEDs.h"
#include "delay.h"
#include "switches.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	
	Init_RGB_LEDs();
	Init_Ultrasonic();
	//Init_PIT(10); //gives us a period of 10 microseconds
	//init_switch();
	//Init_LCD();
	__enable_irq();
	Generate_Trigger();
	Delay(100);
	//while(1){
			//Generate_Trigger();
			//Delay(1000);
	//}
	
	
	
	/*
	Clear_LCD();
  
	Set_Cursor(0,0);
	Print_LCD("Hello World!");
	*/
}
