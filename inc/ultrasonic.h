#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include <MKL25Z4.h>

#define PIN_TRIG_PORT			PORTA
#define PIN_TRIG_PT				PTA
#define PIN_TRIG_SHIFT		( 17 )
#define PIN_TRIG					(1 << PIN_TRIG_SHIFT)

#define PIN_ECHO_PORT			PORTA
#define PIN_ECHO_PT				PTA
#define PIN_ECHO_SHIFT		( 16 )
#define PIN_ECHO          ( 1 << PIN_ECHO_SHIFT)

void Init_Ultrasonic(void);
void Generate_Trigger(void);

#endif
