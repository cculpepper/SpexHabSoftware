#include <msp430.h>
#include <stdint.h>

// required
#define SCL BIT5
#define SDA BIT6
#define READ 0xA1
#define WRITE 0xA0
#define FAILURE -1
#define SUCCESS 0
// required
void sendByte(void);
void sendByteInput(char);
char receiveByte(void);
void sendAck(void);
int receiveAck(void);
void SWI2CStart(void);
void SWI2CStop(void);
int initSwI2C(void);

// required
unsigned char txData = 0;
unsigned char rxData = 0;
unsigned char ackFlag = 0;
unsigned char bitCounter = 0;
unsigned int address = 0; // 12 bit address, upper 4 bits should be 0s.

char bmp180CheckComs(){
	SWI2CStart();
	txData = 0xEE;
	sendByte();
	receiveAck();
	if(!ackFlag)
		return 3;
	//txData = 0xBF;
	sendByteInput(0xD0);
	//_delay_cycles(100); // delay for testing
	//SWI2CStart();
	//sendByteInput(0xD0);
	receiveAck();
	if(!ackFlag)
		return 2;
	SWI2CStop();
	SWI2CStart();
	sendByteInput(0xEF);
	receiveAck();
	if(!ackFlag)
		return FAILURE;
	receiveByte();
	if (rxData == 0x55){
		sendAck();
		return 1;
	}
}



char bmp180ReadBytes(char addr, char reg, int len, char* dest){
	int index;
	index = 0;
	SWI2CStart();

	sendByteInput(addr);
	receiveAck();
	if(!ackFlag)
		return 3;
	//txData = 0xBF;
	sendByteInput(reg);
	//_delay_cycles(100); // delay for testing
	//SWI2CStart();
	//sendByteInput(0xD0);
	receiveAck();
	if(!ackFlag)
		return 2;
	SWI2CStop();
	SWI2CStart();
	sendByteInput(addr+1);
	receiveAck();
	if(!ackFlag)
		return FAILURE;
	while (len--){
		receiveByte();
		dest[index++] = rxData;
		sendAck();
	}
	SWI2CStop();
}



// required
// send byte to slave
void sendByte(void) {
	P3DIR |= SDA;
	bitCounter = 0;
	while(bitCounter < 8) {
		(txData & BIT7) ? (P3OUT |= SDA) : (P3OUT &= ~SDA);
		P3OUT |= SCL;
		txData <<= 1;
		bitCounter++;
		P3OUT &= ~SCL;
	}
	P3OUT |= SDA;
	P3DIR &= ~SDA;
}
void sendByteInput(char input) {
	P3DIR |= SDA;
	bitCounter = 0;
	while(bitCounter < 8) {
		(input & BIT7) ? (P3OUT |= SDA) : (P3OUT &= ~SDA);
		P3OUT |= SCL;
		input <<= 1;
		bitCounter++;
		P3OUT &= ~SCL;
	}
	P3OUT |= SDA;
	P3DIR &= ~SDA;
}
// required
// receive byte from slave
char  receiveByte(void) {
	char res;
	bitCounter = 0;
	while(bitCounter < 8) {
		P3OUT |= SCL;
		res <<= 1;
		bitCounter++;
		if(P3IN & SDA) {
			res |= BIT0;
		}
		P3OUT &= ~SCL;
	}
	return res;
}

void receiveBytePtr(char* ptr) {
	bitCounter = 0;
	while(bitCounter < 8) {
		P3OUT |= SCL;
		*ptr <<= 1;
		bitCounter++;
		if(P3IN & SDA) {
			*ptr |= BIT0;
		}
		P3OUT &= ~SCL;
	}
}
// required
// send master's ACK
void sendAck(void) {
	P3DIR |= SDA;
	(ackFlag) ? (P3OUT &= ~SDA) : (P3OUT |= SDA);
	P3OUT |= SCL;
	P3OUT &= ~SCL;
	P3OUT |= SDA;
	P3DIR &= ~SDA;
}
// required
// receive slave's ACK
void receiveAck(void) {
	P3OUT |= SCL;
	(P3IN & SDA) ? (ackFlag = 0) : (ackFlag = 1);
	P3OUT &= ~SCL;
}
// required
// start condition
void SWI2CStart(void) {
	P3OUT |= SCL;
	P3DIR |= SCL;
	P3OUT |= SCL;
	P3DIR |= SDA;
	P3OUT &= ~SDA;
	P3OUT &= ~SCL;
	P3OUT |= SDA;
	P3DIR &= ~SDA;

}
// required
// stop condition
void SWI2CStop(void) {
	P3DIR |= SDA;
	P3OUT &= ~SDA;
	P3OUT |= SCL;
	P3OUT |= SDA;
	P3DIR &= ~SDA;
}
