#include "uart.h"
#include "gps.h"
struct GPSData {
	char time[7];
	char stat;
	char lat[11];
	char lon[12];
	char speed[6];
	char angle[6];
	char date[7];
}
GPSData gpsData;


char updatePos(){
	 /* 
$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *	

		*/
	int fieldNum;
	char buf[80];
	char count;
	char SearchStr[] = "$GPGGA";
	int index, index2;
	count = 0;
	if (gpsStat >0){
		/*We need to init the gps.*/
		initGps();
	}
	index = 0;
	index2 = 0;
	retryCount = 0;
	getString(buf);
	getString(buf);
	// The second one is to ensure that we have a complete string
	while (retryCount < MAXRETRIES){
		if (strInStr(buf, SearchStr)){
			found = 1;
			break;
		} else {
			getString(buf);
			retryCount++;
			continue;
		}
	}
	if (found == 0){
		return 1;
	}
	// Buf contains the string were looking for. 
	while (buf[index] != ','){
		index++;
	}
	//Points to the comma now. 
	//
	index++;
	//Points to the start of the time feild. 
	if (buf[index] != ','){
		//GPS module doesnt fill in fields so you get ,,,,,,,,,,n,,,,..... Which makes it hard to parse.
		while (buf[index] != ','){
			gpsData->time[index2++] = buf[index++];
		}
		gpsData->time[index2] = 0;
		// we leave the index pointing at the start of the status section. 
		index++;
	} else {
		index++;
		// this should leave the index pointing at the status.
	}
	gpsData->stat = buf[index];
	if (buf[index] != 'A'){
		return 2;
	}
	index += 2;
	// Should now be yointing at the start ef the latitude of a valid string. 
	index2 = 0;
	while (buf[index] != ','){
		gpsData->lat[index2++] = buf[index++];
	} // leaving off with the pointer at the comma at the end of the latitude. 
	index += 1;
	gpsData->lat[index2++] = buf[index++];
	// The north indicator. 
	index++;
	// Should now be yointing at the start ef the longitude of a valid string. 
	index2 = 0;
	while (buf[index] != ','){
		gpsData->lon[index2++] = buf[index++];
	} // leaving off with the pointer at the comma at the end of the longitude. 
	index ++;
	gpsData->lon[index2++] = buf[index++];
	// The east indicator. 
	index++;
	// Meh. Dont feel like implementing the rest...
			
	return 0;
}
char initGps(void){
	gpsStat = -1;  /* Not updated*/ 
	/*Need to select PTB4 as a rxd pin for uart0*/
	 /* Thats ALT3, 0x3*/ 
	PORTB_PCR4 = PORT_PCR_MUX(3) || PORT_PCR_ISF_MASK;
	/*PORTB_PCR1 = PORT_PCR_MUX(2); Dont need TX for gps*/
/* Also need to disable the rxd for the other pins. I think?*/ 
	/*Might as well initialize the UART too. */
	SIM_SOPT5 &= ~ SIM_SOPT5_UART0_EXTERN_MASK_CLEAR;
	SIM_SCGC4 |= SIM_SCGC4_UART0CGC_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTECGC_MASK;  /* THIS IS WRONG*/ 
	UART_BDH_REG = UART_BDH_38400;
	UART_BDL_REG = UART_BDL_38400;
	UART_C1_REG = UART_C1_8N1;
	UART_C3_REG = UART_C3_NO_TXINV;
	UART_C4_REG = UART_C4_NO_DMA;
	UART_S2_REG = UART_S2_NO_RXINV_BRK10_NO_LBKDETECT;
	UART_C2_REG = UART_C2_T_RI;
}

	



		
