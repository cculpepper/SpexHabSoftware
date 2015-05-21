// blink.c
//
//****************************************************************************************************
// Author:
// 	Nipun Gunawardena
//
// Credits:
//	http://hci.rwth-aachen.de/msp430	
//
// Requirements:
// 	Requires msp430-gcc
//
// Description:
//	Blinking light program
//
// Notes:
//	
//
//****************************************************************************************************




// Includes ------------------------------------------------------------------------------------------
#include <msp430g2553.h>




// Defines -------------------------------------------------------------------------------------------
#define LED1 BIT0 			// Red LED on LaunchPad
#define LED_OUT P1OUT
#define LED_DIR P1DIR




// Functions -----------------------------------------------------------------------------------------
void delay(unsigned long count){
	volatile long i;        // Declare i as volatile int
	for(i=0;i<count;i++);  // Repeat x times
}




// Main ----------------------------------------------------------------------------------------------
int main(void){
	// Turn off watchdog timer
	WDTCTL = WDTPW|WDTHOLD;

	// Set LED output as 0
	LED_OUT  = 0x00;

	// Set direction
	LED_DIR = LED1;

	// Disable interrupts
	//P1IES  = 0x00;		// Apparently not necessary. Sets interrupt transition setting
	//P1IE   = 0x00;		// Turn interrupts off

	//  Turn LED1 on
	LED_OUT = LED1;

	// Loop
	while (1){
		//  Toggle the LED ouput pins
		LED_OUT = LED1;
		delay(12800);

		LED_OUT = 0;
		delay(12800);
	}
}
