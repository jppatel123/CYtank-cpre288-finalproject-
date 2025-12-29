/*
*
*   uart.h
*
*   Used to set up the UART
*   uses UART1 at 115200
*
*
*   @author Dane Larson
*   @date 07/18/2016
*   Phillip Jones updated 9/2019, removed WiFi.h
*/

#ifndef UART_H_
#define UART_H_

#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void uart_init(void);

void uart_sendChar(char data);
int cyBot_getByte_available(void);
char uart_receive(void);
void uart_sendStr(const char *data);
//extern volatile char receivedChar;
//void uart_interrupt_init(void);

#endif /* UART_H_ */
