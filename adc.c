



#include<stdint.h>
#include "adc.h"

void adc_init(void) {
    SYSCTL_RCGCADC_R |= 0x01;        // Enable ADC0 clock
    SYSCTL_RCGCGPIO_R |= 0x02;       // Enable Port B clock
    while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}; // Wait for Port B ready

    GPIO_PORTB_AFSEL_R |= 0x10;      // Enable alternate function on PB4
    GPIO_PORTB_DEN_R &= ~0x10;       // Disable digital on PB4
    GPIO_PORTB_AMSEL_R |= 0x10;      // Enable analog on PB4

    ADC0_ACTSS_R &= ~0x08;           // Disable SS3
    ADC0_EMUX_R &= ~0xF000;          // Software trigger
    ADC0_SSMUX3_R = 10;              // Set channel AIN10 (PB4)
    ADC0_SSCTL3_R = 0x06;            // End of sequence, no interrupt
    ADC0_ACTSS_R |= 0x08;            // Enable SS3

    ADC0_SAC_R = 0x04;
}

     uint16_t adc_read(void) {
    ADC0_PSSI_R = 0x08;              // Start SS3
    while ((ADC0_RIS_R & 0x08) == 0) {}; // Wait for conversion
    uint16_t result = ADC0_SSFIFO3_R & 0xFFF; // Read 12-bit result
    ADC0_ISC_R = 0x08;               // Clear completion flag
    return result;
}

