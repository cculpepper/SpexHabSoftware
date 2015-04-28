//#include "WAIT1.h"
//#include "DA1.h"
#include "cw.h"
#include "msp430.h"
//#include "TI1.h"
//#include "LED1.h"
#define DITTIME 7000 //Time in milliseconds for a dit
#define DAHTIME (3 * (DITTIME))
char MSP430Delay(int cycles){
	TA1CCTL0 = 0;
	TA1R = 0;
	  TA1CCR0 = cycles;
	  TA1CTL |= TASSEL_1 | MC_1;	//Use ACLK as source for timer
	  TA1CTL |= MC_1;	//Use UP mode timer
		TA1CCTL0 |= CCIE; // CCR0 interrupt enabled
	  __bis_SR_register(LPM0_bits + GIE);                // Enter LPM0 w/ Interrupt
	  TA1CCTL0 &= ~CCIE;
	  return 0;
}
const char CwLetterData[26] = {
	 /* 3 bit length, 5 bits of data. Can do everything but symbols.....*/ 
	/* Bits are right aligned, 1 is dit, 0 is dah a is 10 */ 
	0x50, 
	0x8E,
	0x8A,
	0x6C,
	0x30,
	0x9A,
	0x64,
	0x9E,
	0x58,
	0x90,
	0x68,
	0x96,
	0x40,
	0x48,
	0x60,
	0x92,
	0x9A,
	0x74,
	0x7C,
	0x20,
	0x78,
	0x9C,
	0x70,
	0x8C,
	0x88,
	0x86
};  
const char CwNumberData[10] = {
	0xB0,
	0xB8,
	0xBC,
	0xBE,
	0xBF,
	0xAF,
	0xA7,
	0xA3,
	0xA1,
	0xA0};



char cwSend(char* data, int len){
	//sends stuff...
	char charLen;
	char currChar;
	char* cwDataPtr;
	cwDataPtr = data;
	while((data+len) > cwDataPtr){
		currChar = *cwDataPtr;
		if (currChar >= 'A' && currChar <= 'Z'){
			currChar = CwLetterData[currChar - 'A']; /* This sets the current char to the morse binary representation of the letter if it is a letter. */ 
		} else if (currChar >= '0' && currChar <= '9'){
			currChar = CwNumberData[currChar - '0'];
		} else {
			currChar = 0;  /* Because I dont know what to do...*/ 
		}

		charLen = (currChar >> 5); /* This is the length of the morse code char, in the top 3 bits*/ 
		while ( charLen > 0 ){
			LED1_ON();
			if (currChar & ( 0x80 >> (5-charLen))){
				/* Wait for a dit*/
				MSP430Delay(DITTIME);
			} else {
				MSP430Delay(DAHTIME);
			}
			LED1_OFF();
			/* A little bit crazy, passes to the sender, the current bit. Ands the current morse byte with a shifted 1  to get the current character*/ 
			MSP430Delay(DITTIME);
			charLen--;
		}
		cwDataPtr++;
		MSP430Delay(DAHTIME);



	}
	for (charLen = 0; charLen < 10; charLen++){
		LED1_ON();
		MSP430Delay(5000);
		LED1_OFF();
		MSP430Delay(5000);
	}
}
 
