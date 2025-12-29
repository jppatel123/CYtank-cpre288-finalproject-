
#include "button.h"



void button_init() {
	static uint8_t initialized = 0;

	//Check if already initialized
	if(initialized){
		return;
	}

	SYSCTL_RCGCGPIO_R |= 0x10;
	while((SYSCTL_RCGCGPIO_R & 0x10)==0) {};

	// 2) Set the buttons as inputs, do not modify other PORTE wires
	 GPIO_PORTE_DIR_R &= ~0x0F;
	
	// 3) Enable digital functionality for button inputs, 

	GPIO_PORTE_DEN_R |= 0x0F;

	
	initialized = 1;
}


uint8_t button_getButton() {


	
    uint8_t data = GPIO_PORTE_DATA_R & 0x0F;
    if ((data & 0x08) == 0) return 4;
    else if ((data & 0x04) == 0) return 3;
    else if ((data & 0x02) == 0) return 2;
    else if ((data & 0x01) == 0) return 1;
    else return 0;
	
	return 0; 
}





