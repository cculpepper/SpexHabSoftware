#ifndef LED
#define LED

#define LED1INIT() P1DIR |= 0x01
#define LED1_ON() P1OUT |= 0x01
#define LED1_TOGGLE() P1OUT ^= 0x01
#define LED1_OFF() P1OUT &= ~0x01

#define LED2 0x01<<6
#define LED2INIT() P4DIR |= LED2
#define LED2_ON() P4OUT |= LED2
#define LED2_TOGGLE() P4OUT ^= LED2
#define LED2_OFF() P4OUT &= ~LED2
#endif
