#include <MKL25Z4.h>

void Delay (uint32_t dly) {
  volatile uint32_t t;
	for (t=dly*10000; t>0; t--)
		;
}


void Delay_us(volatile unsigned int time_del) {
	// This is a very imprecise and fragile implementation!
	time_del = 9*time_del + time_del/2; 
	while (time_del--) {
		;
	}
}


// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
