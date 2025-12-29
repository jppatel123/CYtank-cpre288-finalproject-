

#ifndef TIMER_H_
#define TIMER_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"


void timer_init(void);


void timer_stop(void);

void timer_pause(void);

void timer_resume(void);


unsigned int timer_getMillis(void);


unsigned int timer_getMicros(void);


void timer_waitMillis(unsigned int delay_time);


void timer_waitMicros(unsigned int delay_time);


void timer_fireEvery(void (*f)(void), int millis);


void timer_fireOnce(void (*f)(void), int millis);


void timer_fireFor(void (*f)(void), int millis, int times);


static void timer_clockTickHandler();

#endif /* TIMER_H_ */
