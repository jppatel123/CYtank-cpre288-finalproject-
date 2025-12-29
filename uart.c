






#include "uart.h"
//volatile char receivedChar;
void uart_init(void){

	SYSCTL_RCGCUART_R|=0x02;
	SYSCTL_RCGCGPIO_R|=0x02;
	 UART1_CTL_R &= ~UART_CTL_UARTEN;
	  UART1_IBRD_R=8;
	    UART1_FBRD_R=44;
	    UART1_LCRH_R=UART_LCRH_WLEN_8|UART_LCRH_FEN;
	    UART1_CC_R= 0x0;
	    UART1_CTL_R = UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN;
	    GPIO_PORTB_AFSEL_R |= 0x03;
	    GPIO_PORTB_PCTL_R &= ~0x000000FF;
	    GPIO_PORTB_PCTL_R |= 0x00000011;
	    GPIO_PORTB_DEN_R |= 0x03;
	    GPIO_PORTB_DIR_R |= 0x02;
	    GPIO_PORTB_DIR_R &= ~0x01;





//
	UART1_ICR_R = UART_ICR_RXIC;
	UART1_IM_R |=UART_IM_RXIM;
	NVIC_EN0_R |=(1<<6);





}

void uart_sendChar(char data){
	while(UART1_FR_R & UART_FR_TXFF)
	{

	}
	  UART1_DR_R = data;



}

char uart_receive(void){
    while(UART1_FR_R & UART_FR_RXFE)
        {

        }
     return (char)(UART1_DR_R & 0xFF);
}

void uart_sendStr(const char *data){
	while(*data!='\0')
	{
	    uart_sendChar(*data);
	    data++;
	}
}

int cyBot_getByte_available(void)
{
    // RXFE = Receive FIFO Empty
    // If RXFE == 0 → data is available
    if ((UART1_FR_R & UART_FR_RXFE) == 0)
    {
        return 1;   // Data available
    }
    else
    {
        return 0;   // No data
    }
}


// void UART1_Handler(void)
//        {
//
//            if(UART1_MIS_R & UART_MIS_RXMIS)
//            {
//                 receivedChar = (char)(UART1_DR_R & 0XFF);
//
//                 uart_sendChar(receivedChar);
//
//                 if(receivedChar=='\r')
//                 {
//                     uart_sendChar('\r');
//                      uart_sendChar('\n');
//                 }
//                 UART1_ICR_R = UART_ICR_RXIC;
//
//            }
//        }
//
//
// void uart_interrupt_init(void)
// {
//     UART1_ICR_R = UART_ICR_RXIC;       // Clear RX interrupt
//     UART1_IM_R |= UART_IM_RXIM;        // Enable RX interrupt
//     NVIC_EN0_R |= (1 << 6);            // Enable UART1 interrupt in NVIC (vector 22)
//     IntRegister(INT_UART1, UART1_Handler); // Register the interrupt handler
// }


