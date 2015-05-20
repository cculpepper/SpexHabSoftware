#ifdef SWI2C
#define SWI2C
#include <msp430.h>
#include <stdint.h>

// required
// required
void sendByte(void);
void sendByteInput(char);
char receiveByte(void);
void sendAck(void);
int receiveAck(void);
void SWI2CStart(void);
void SWI2CStop(void);
#endif
