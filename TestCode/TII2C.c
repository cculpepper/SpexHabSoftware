//******************************************************************************
//   MSP430 USCI I2C Transmitter and Receiver
//
//  Description: This code configures the MSP430's USCI module as
//  I2C master capable of transmitting and receiving bytes.
//
//  ***THIS IS THE MASTER CODE***
//
//                    Master
//                 MSP430F2619
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |                 |
//            |                 |
//            |         SDA/P3.1|------->
//            |         SCL/P3.2|------->
//
// Note: External pull-ups are needed for SDA & SCL
//
// Uli Kretzschmar
// Texas Instruments Deutschland GmbH
// November 2007
// Built with IAR Embedded Workbench Version: 3.42A
//******************************************************************************

#include "msp430.h"                     // device specific header
#include "stdint.h"
//#include "msp430x22x4.h"
//#include "msp430x23x0.h"
//#include "msp430xG46x.h"
// ...                                         // more devices are possible

#include "TII2C.h"

signed char byteCtr;
unsigned char *TI_receive_field;
unsigned char *TI_transmit_field;

//------------------------------------------------------------------------------
// void TI_USCI_I2C_receiveinit(unsigned char slave_address,
//                              unsigned char prescale)
//
// This function initializes the USCI module for master-receive operation.
//
// IN:   unsigned char slave_address   =>  Slave Address
//       unsigned char prescale        =>  SCL clock adjustment
//-----------------------------------------------------------------------------
void TI_USCI_I2C_receiveinit(unsigned char slave_address,
		unsigned char prescale){
	P1SEL1 |= BIT6 | BIT7;            // Assign I2C pins to USCI_B0
	UCB0CTLW0 = UCSWRST;                        // Enable SW reset
	UCB0CTLW0 |= UCMST | UCMODE_3 | UCSYNC;       // I2C Master, synchronous mode
	UCB0CTLW1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
	UCB0BR0 = prescale;                         // set prescaler
	UCB0BR1 = 0;
	UCB0I2CSA = slave_address;                  // set slave address
	UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
	UCB0IE = UCNACKIE | UCRXIE;
	//IE2 = UCB0RXIE;                            // Enable RX interrupt
}

//------------------------------------------------------------------------------
// void TI_USCI_I2C_transmitinit(unsigned char slave_address,
//                               unsigned char prescale)
//
// This function initializes the USCI module for master-transmit operation.
//
// IN:   unsigned char slave_address   =>  Slave Address
//       unsigned char prescale        =>  SCL clock adjustment
//------------------------------------------------------------------------------
void TI_USCI_I2C_transmitinit(unsigned char slave_address,
		unsigned char prescale){
	P3SEL1 |= SDA_PIN + SCL_PIN;                 // Assign I2C pins to USCI_B0
	UCB0CTL1 = UCSWRST;                        // Enable SW reset
	UCB0CTLW0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
	UCB0BR0 = prescale;                         // set prescaler
	UCB0BR1 = 0;
	UCB0I2CSA = slave_address;                  // Set slave address
	UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
	UCB0IE = UCNACKIE | UCRXIE;                            // Enable TX ready interrupt
}

//------------------------------------------------------------------------------
// void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field)
//
// This function is used to start an I2C commuincation in master-receiver mode.
//
// IN:   unsigned char byteCount  =>  number of bytes that should be read
//       unsigned char *field     =>  array variable used to store received data
//------------------------------------------------------------------------------
void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field){
	TI_receive_field = field;
	if ( byteCount == 1 ){
		byteCtr = 0 ;
		__disable_interrupt();
		UCB0CTL1 |= UCTXSTT;                      // I2C start condition
		while (UCB0CTL1 & UCTXSTT);               // Start condition sent?
		UCB0CTL1 |= UCTXSTP;                      // I2C stop condition
		__enable_interrupt();
	} else if ( byteCount > 1 ) {
		byteCtr = byteCount - 2 ;
		UCB0CTL1 |= UCTXSTT;                      // I2C start condition
	} else
		while (1);                                // illegal parameter
}

//------------------------------------------------------------------------------
// void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field)
//
// This function is used to start an I2C commuincation in master-transmit mode.
//
// IN:   unsigned char byteCount  =>  number of bytes that should be transmitted
//       unsigned char *field     =>  array variable. Its content will be sent.
//------------------------------------------------------------------------------
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field){
	TI_transmit_field = field;
	byteCtr = byteCount;
	UCB0CTL1 |= UCTR + UCTXSTT;                 // I2C TX, start condition
}

//------------------------------------------------------------------------------
// unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address)
//
// This function is used to look for a slave address on the I2C bus.
//
// IN:   unsigned char slave_address  =>  Slave Address
// OUT:  unsigned char                =>  0: address was not found,
//                                        1: address found
//------------------------------------------------------------------------------
unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address){
	unsigned char slaveadr_bak, returnValue;
	uint16_t ucb0i2cie;
	ucb0i2cie = UCB0IE;                      // restore old UCB0I2CIE
	//ie2_bak = IE2;                              // store IE2 register
	slaveadr_bak = UCB0I2CSA;                   // store old slave address
	UCB0IE &= ~ UCNACKIE;                    // no NACK interrupt
	UCB0I2CSA = slave_address;                  // set slave address
	UCB0IE &= ~(UCTXIE + UCRXIE);              // no RX or TX interrupts
	__disable_interrupt();
	UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;       // I2C TX, start condition
	while (UCB0CTL1 & UCTXSTP);                 // wait for STOP condition

	returnValue = !(UCB0STAT & UCNACKIFG);
	__enable_interrupt();
	//IE2 = ie2_bak;                              // restore IE2
	UCB0I2CSA = slaveadr_bak;                   // restore old slave address
	UCB0IE = ucb0i2cie;                      // restore old UCB0CTL1
	return returnValue;                         // return whether or not
	// a NACK occured
}

//------------------------------------------------------------------------------
// unsigned char TI_USCI_I2C_notready()
//
// This function is used to check if there is commuincation in progress.
//
// OUT:  unsigned char  =>  0: I2C bus is idle,
//                          1: communication is in progress
//------------------------------------------------------------------------------
unsigned char TI_USCI_I2C_notready(){
	return (UCB0STAT & UCBBUSY);
}
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
	{
		case USCI_NONE:          break;         // Vector 0: No interrupts
		case USCI_I2C_UCALIFG:   break;         // Vector 2: ALIFG
		case USCI_I2C_UCNACKIFG: break;         // Vector 4: NACKIFG
		case USCI_I2C_UCSTTIFG:  break;         // Vector 6: STTIFG
		case USCI_I2C_UCSTPIFG:                 // Vector 8: STPIFG
					 //TXData = 0;
					 UCB0IFG &= ~UCSTPIFG;                 // Clear stop condition int flag
					 break;
		case USCI_I2C_UCRXIFG3:  break;         // Vector 10: RXIFG3
		case USCI_I2C_UCTXIFG3:  break;         // Vector 12: TXIFG3
		case USCI_I2C_UCRXIFG2:  break;         // Vector 14: RXIFG2
		case USCI_I2C_UCTXIFG2:  break;         // Vector 16: TXIFG2
		case USCI_I2C_UCRXIFG1:  break;         // Vector 18: RXIFG1
		case USCI_I2C_UCTXIFG1:  break;         // Vector 20: TXIFG1
		case USCI_I2C_UCRXIFG0:
					 if (UCB0STAT & UCNACKIFG){            // send STOP if slave sends NACK
						 UCB0CTL1 |= UCTXSTP;
						 UCB0STAT &= ~UCNACKIFG;
					 }
					 if ( byteCtr == 0 ){
						 UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
						 *TI_receive_field = UCB0RXBUF;
						 TI_receive_field++;
					 }
					 else {
						 *TI_receive_field = UCB0RXBUF;
						 TI_receive_field++;
						 byteCtr--;
					 }

					 break;
		case USCI_I2C_UCTXIFG0:                 // Vector 24: TXIFG0

					 if (byteCtr == 0){
						 UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
						 //IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
					 }
					 else {
						 UCB0TXBUF = *TI_transmit_field;
						 TI_transmit_field++;
						 byteCtr--;
					 }

		case USCI_I2C_UCBCNTIFG: break;         // Vector 26: BCNTIFG
		case USCI_I2C_UCCLTOIFG: break;         // Vector 28: clock low timeout
		case USCI_I2C_UCBIT9IFG: break;         // Vector 30: 9th bit
		default: break;
	}
}
