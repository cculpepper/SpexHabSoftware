
/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR59xx Demo - eUSCI_A0 UART echo at 9600 baud using BRCLK = 32768Hz
//
//  Description: This demo echoes back characters received via a PC serial port.
//  ACLK is used as a clock source and the device is put in LPM3
//  Note that level shifter hardware is needed to shift between RS232 and MSP
//  voltage levels.
//
//  The example code shows proper initialization of registers
//  and interrupts to receive and transmit data.
//  To test code in LPM3, disconnect the debugger and use an external supply
//  Otherwise use LPM0
//
//  ACLK = 32768Hz, MCLK =  SMCLK = default DCO
//
//                MSP430FR5969
//             -----------------
//            |                 |--LFXTIN (32768Hz reqd.)
//            |                 |--LFXTOUT
//            |                 |
//       RST -|     P2.0/UCA0TXD|----> PC (echo)
//            |                 |
//            |                 |
//            |     P2.1/UCA0RXD|<---- PC
//            |                 |
//
//   P. Thanigai
//   Texas Instruments Inc.
//   August 2012
//   Built with IAR Embedded Workbench V5.40 & Code Composer Studio V5.5
//******************************************************************************


#include "msp430.h"
#include "cw.h"
#include "LED1.h"
#define MAXRXBUFF 80
#define MAXTXBUFF 80
char UARTRXBuf[MAXRXBUFF];
char UARTTXBuf[MAXTXBUFF];
int UARTTXLen;
int UARTRXLen;
int UARTTXOutIndex;
char UARTSending; // Used to not transmit until the string is there.


int I2CRXIndex;
char** I2CRXBuffer;
int I2CRXLen;

I2CInit(void){
	  P1SEL1 |= BIT6 | BIT7;                    // I2C pins

	  // Disable the GPIO power-on default high-impedance mode to activate
	  // previously configured port settings
	  PM5CTL0 &= ~LOCKLPM5;

	  // Configure USCI_B0 for I2C mode
	  UCB0CTLW0 |= UCSWRST;                     // Software reset enabled
	  UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC;   // I2C mode, Master mode, sync
	  UCB0CTLW1 |= UCASTP_2;                    // Automatic stop generated
	                                            // after UCB0TBCNT is reached
	  UCB0BRW = 0x0008;                         // baudrate = SMCLK / 8
	  //UCB0TBCNT = 0x0005;                       // number of bytes to be received
	  //UCB0I2CSA = 0x0048;                       // Slave address
	  //UCB0CTL1 &= ~UCSWRST;
	  //UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIE;

	  //The above will need to be done when recieving bytes.

}
char I2CReadByte(char add, char reg){
	char ret;
	UCB0TBCNT = 0x0001;                       // number of bytes to be received
	UCB0I2CSA = add;                       // Slave address
	I2CRXBuffer = &ret;
	UCB0TXBUF = reg;


	UCB0CTL1 &= ~UCSWRST;
	//UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIE;

}
void PCUartInit(void){
	  // Configure GPIO
	  P2SEL1 |= BIT0 | BIT1;                    // USCI_A0 UART operation
	  P2SEL0 &= ~(BIT0 | BIT1);
	  PJSEL0 |= BIT4 | BIT5;                    // For XT1

	  // Disable the GPIO power-on default high-impedance mode to activate
	  // previously configured port settings
	  PM5CTL0 &= ~LOCKLPM5;

	  // XT1 Setup
	  CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
	  CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
	  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
	  CSCTL4 &= ~LFXTOFF;
	  do
	  {
	    CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
	    SFRIFG1 &= ~OFIFG;
	  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
	  CSCTL0_H = 0;                             // Lock CS registers

	  // Configure USCI_A0 for UART mode
	  UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
	  UCA0CTLW0 |= UCSSEL__ACLK;                // CLK = ACLK
	  UCA0BR0 = 3;                              // 9600 baud
	  UCA0MCTLW |= 0x5300;                      // 32768/9600 - INT(32768/9600)=0.41
	                                            // UCBRSx value = 0x53 (See UG)
	  UCA0BR1 = 0;
	  UCA0CTL1 &= ~UCSWRST;                     // Initialize eUSCI
	  UCA0IE |= UCRXIE | UCTXIE;
	  UARTSending = 0;
	  UARTTXLen = 0;
	  UARTRXLen = 0;
	  UARTTXOutIndex =0;
}
void putString(char* str){
	int i;
	char curr;
	i = 0;
	curr = str[i];
	while (curr){
		UARTTXBuf[UARTTXLen++] = curr;
		curr = str[++i];
	}
	UARTSending = 1;
	UCA0IE |=  UCTXIE;
}
void putChar(char ch){
	UARTTXBuf[UARTTXLen++] = ch;
}
void putNum(int num){
	char ch;
	ch = (num / 10000) + '0';
	putChar(ch);
	num %= 10000;
	ch = (num / 1000) + '0';
	putChar(ch);
	num %= 1000;
	ch = (num / 100) + '0';
	putChar(ch);
	num %= 100;
	ch = (num / 10) + '0';
	putChar(ch);
	num %= 10;
	ch = num + '0';
	putChar(ch);

}
int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;                 // Stop Watchdog
  PCUartInit();
  LED2INIT();
           // Enable USCI_A0 RX interrupt

  //__bis_SR_register(LPM3_bits | GIE);       // Enter LPM3, interrupts enabled


  __no_operation(); // For debugger
  //I2CInit()
  for (;;){
	  putString("AB1TJ IS OVER ");
	  putNum(9000);
	  putString(" GOOD \r\n");
  cwSend("AB1TJ", 5);
  putString("AB1TJ");
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  //switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG)){
    //if ( USCI_NONE){}
    if ( USCI_UART_UCRXIFG){
      if (UARTRXLen < MAXRXBUFF){
    	  UARTRXBuf[UARTRXLen++] = UCA0RXBUF;
      }
    }
    if( USCI_UART_UCTXIFG){
    	if (UARTSending){
			if (UARTTXOutIndex < UARTTXLen){
			    while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = UARTTXBuf[UARTTXOutIndex++];
			} else {
				UARTSending = 0;
				UCA0IE &=  ~UCTXIE;
				UARTTXOutIndex =0;
				UARTTXLen = 0;
			}
    	} else {
			UCA0IE &=  ~UCTXIE;
    	}
    }
    //case USCI_UART_UCSTTIFG: break;
    //case USCI_UART_UCTXCPTIFG: break;

  //LED1_TOGGLE();
}
// Timer A0 interrupt service routine

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A_Delay (void){

	LPM0_EXIT;
	TA1CCR0 = 0;//Stop the counter.


}
#pragma vector= TRAPINT_VECTOR
__interrupt void TRAPINT_ISR(void)

{
  __no_operation();
}
