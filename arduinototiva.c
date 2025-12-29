#include <stdint.h>
#include "open_interface.h"
#include "arduinototiva.h"

void UART2_Init(void){
    SYSCTL_RCGCUART_R |= (1 << 2);   // Enable UART2 clock
    SYSCTL_RCGCGPIO_R |= (1 << 3);   // Enable Port D clock

    // --- Unlock PD7 (required for UART2 TX) ---
    GPIO_PORTD_LOCK_R = 0x4C4F434B;
    GPIO_PORTD_CR_R |= 0x80;         // Allow changes to PD7

    GPIO_PORTD_AFSEL_R |= 0xC0;      // PD6 (RX) PD7 (TX)
    GPIO_PORTD_PCTL_R &= ~0xFF000000;
    GPIO_PORTD_PCTL_R |= 0x11000000; // PD6=U2RX, PD7=U2TX
    GPIO_PORTD_DEN_R |= 0xC0;        // Digital enable PD6 & PD7
    GPIO_PORTD_AMSEL_R &= ~0xC0;     // Disable analog

    UART2_CTL_R = 0;                 // Disable UART
    UART2_IBRD_R = 104;              // 9600 baud
    UART2_FBRD_R = 11;
    UART2_LCRH_R = 0x70;             // 8-bit, FIFO enabled
    UART2_CTL_R = 0x301;             // Enable UART, TX & RX
}

void UART2_WriteChar(char c){
    while(UART2_FR_R & 0x20);        // Wait until TX FIFO not full
    UART2_DR_R = c;
}

void UART2_WriteString(char *str){
    while(*str){
        UART2_WriteChar(*str++);
    }
}
