#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"

#define BUS_CLOCK_FREQUENCY (24E6)
#define TICK_FREQUENCY (1E3)

#define LED_ON_TIME_TICKS (500)
#define LED_UPDATE_PERIOD (TICK_FREQUENCY/50)

#define PWM_MAX_COUNT (600)

void Init_PIT(unsigned period);
void Start_PIT(void);
void Stop_PIT(void);

#endif
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
