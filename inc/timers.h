#ifndef TIMERS_H
#define TIMERS_H
#include <MKL25Z4.h>

#define BUS_CLOCK_FREQUENCY (24E6)
#define TICK_FREQUENCY (1E3)

#define MAX_PIT (0xFFFFFFFF)
#define LED_ON_TIME_TICKS (500)
#define LED_UPDATE_PERIOD (TICK_FREQUENCY/50)

#define PWM_MAX_COUNT (600)

void Init_PIT(unsigned period);
void Start_PIT(void);
void Stop_PIT(void);
void Set_PIT_Period(uint32_t period);
uint32_t Get_PIT_Count(void);


#endif
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
