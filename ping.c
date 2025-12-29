


#include "ping.h"
#include "Timer.h"

#include <stdbool.h>
#include "driverlib/interrupt.h"  // For IntRegister, IntEnable, IntMasterEnable

#define BIT3 0x08

volatile int start_time = 0;
volatile int stop_time = 0;

void ping_init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x02;  // Enable clock for Port B
    SYSCTL_RCGCTIMER_R |= 0x08; // Enable clock for Timer 3
    timer_waitMillis(10);

    GPIO_PORTB_DEN_R |= BIT3;   // Enable digital function on PB3
    GPIO_PORTB_DIR_R &= ~BIT3;  // PB3 input
    GPIO_PORTB_AFSEL_R |= BIT3; // Alternate function
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FFF) | 0x00007000;

    TIMER3_CTL_R &= ~0x100;     // Disable Timer3B during config
    TIMER3_CFG_R = 0x04;        // 16-bit mode
    TIMER3_TBMR_R = 0x07;       // Capture mode + edge-time
    TIMER3_CTL_R |= 0x0C00;     // Capture both edges
    TIMER3_TBILR_R = 0xFFFF;    // Max interval
    TIMER3_TBPR_R = 0xFF;       // Prescaler
    TIMER3_IMR_R |= 0x0400;     // Enable capture interrupt

    IntRegister(INT_TIMER3B, TIMER3B_Handler);
    IntEnable(INT_TIMER3B);
    IntMasterEnable();
}

void pulse_ping(void)
{
    // Configure PB3 as GPIO output
    GPIO_PORTB_AFSEL_R &= ~BIT3;
    GPIO_PORTB_DIR_R |= BIT3;
    GPIO_PORTB_DATA_R &= ~BIT3;
    timer_waitMicros(2);

    GPIO_PORTB_DATA_R |= BIT3;  // Trigger pulse
    timer_waitMicros(50);
    GPIO_PORTB_DATA_R &= ~BIT3;
    timer_waitMicros(2);

    // Switch PB3 back to input and timer capture
    GPIO_PORTB_DIR_R &= ~BIT3;
    GPIO_PORTB_AFSEL_R |= BIT3;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FFF) | 0x00007000;

    TIMER3_CTL_R |= 0x100; // Enable Timer3B
}

void TIMER3B_Handler(void)
{
    TIMER3_ICR_R = 0x400; // Clear interrupt
    if (start_time == 0)
    {
        start_time = TIMER3_TBR_R; // Rising edge
    }
    else
    {
        stop_time = TIMER3_TBR_R;  // Falling edge
        TIMER3_CTL_R &= ~0x100;    // Disable Timer3B
    }
}
