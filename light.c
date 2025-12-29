#include <stdint.h>
#include "light.h"
#include "open_interface.h"
void blink()
{
    int i;
    GPIO_PORTF_DATA_R |= 0x02;   // LED ON
    for(i=0;i<500000;i++);

    GPIO_PORTF_DATA_R &= ~0x02;  // LED OFF
    for(i=0;i<500000;i++);
}
void light_init()
{

    // Enable clock for PORTF
    SYSCTL_RCGCGPIO_R |= 0x20;          // Bit 5 = Port F

    // Small delay for clock stabilization
    volatile int delay = SYSCTL_RCGCGPIO_R;

    // Unlock PF0 (not necessary for PF1 but recommended)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x02;            // Allow changes to PF1

    // Set PF1 as OUTPUT
    GPIO_PORTF_DIR_R |= 0x02;

    // Enable digital function on PF1
    GPIO_PORTF_DEN_R |= 0x02;

}
