



#include "servo.h"
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x02;
    while ((SYSCTL_PRGPIO_R & 0x02) == 0);


    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R &= ~0x00F00000;
    GPIO_PORTB_PCTL_R |= 0x00700000;


    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;

    TIMER1_CTL_R &= ~0x100;
    TIMER1_CFG_R = 0x04;
    TIMER1_TBMR_R |= 0x0A;
    TIMER1_CTL_R &= ~0x4000;
    TIMER1_TBPR_R  |= 0x04;
    TIMER1_TBILR_R = 0xE200;
    TIMER1_TBMATCHR_R = 0xA700;
    TIMER1_TBPMR_R = 0x04;
    TIMER1_CTL_R |= 0x100;
}









int servo_move(int degrees) {
    if (degrees < 0) degrees = 0;
    if (degrees > 180) degrees = 180;

    // Correct match values for 0° and 180°
    int32_t match_deg0 = 0x4CB50;   // 304,000 ticks for 0°
    int32_t match_deg180 =0x45000; // 288,000 ticks for 180°

    float ratio = degrees / 180.0;
    int32_t match_value = (int32_t)(match_deg0 + ratio * (match_deg180 - match_deg0));

    TIMER1_TBMATCHR_R = match_value & 0xFFFF; // Lower 16 bits
    TIMER1_TBPMR_R = (match_value >> 16) & 0xFF; // Upper 8 bits

    return match_value;
}

