#ifndef LED1
#define LED1

#define LED1INIT() P1DIR |= 0x01
#define LED1_ON() P1OUT |= 0x01
#define LED1_TOGGLE() P1OUT ^= 0x01
#define LED1_OFF() P1OUT &= ~0x01

#endif
