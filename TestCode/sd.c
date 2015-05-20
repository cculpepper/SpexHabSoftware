#include <msp430.h>
#include <stdint.h>
#include "dsLib.h"
#include "./FatFS/ff.h"
#include "./FatFS/diskio.h"

/*
 *
 * IMPORTANT UPDATING INFORMATION!
 * When updating FatFS, two things need to be remembered:
 * - The diskio.c file is unique to the MSP430FR5969. Don't use included ChaN included diskio.c file
 * - _USE_STRFUNC in ffconf.h needs to be defined as 1 (~ line 36). Compilation errors will happen otherwise
 *
 */


FATFS sdVolume;		// FatFs work area needed for each volume
/*FIL logfile;		// File object needed for each open file*/
uint16_t fp;		// Used for sizeof

uint8_t status = 17;	// Status variable that should change if successful


int SDOpenFile ( FIL *logfile)
{
	  /*WDTCTL = WDTPW | WDTHOLD;       // Stop WDT*/

	  /*// Prepare LEDs*/
	  /*P1DIR = 0xFF ^ (BIT1 | BIT3);             // Set all but P1.1, 1.3 to output direction*/
	  /*P1OUT |= BIT0;							// LED On*/
	  /*P4DIR = 0xFF;							  	// P4 output*/
	  /*P4OUT |= BIT6;							// P4.6 LED on*/

	  /*// Prepare I2C and clock pins*/
	  /*P1SEL1 |= BIT6 | BIT7;                    // I2C pins*/
	  /*PJSEL0 |= BIT4 | BIT5;					// Set J.4 & J.5 to accept crystal input for ACLK*/

	  /*// Prepare P1.1 and P1.3 for switch*/
	  /*P1OUT = BIT1 | BIT3;                      // Pull-up resistor on P1.1, 1.3*/
	  /*P1REN = BIT1 | BIT3;                      // Select pull-up mode for P1.1, 1.3*/

	  /*// Disable the GPIO power-on default high-impedance mode to activate*/
	  /*// previously configured port settings*/
	  /*PM5CTL0 &= ~LOCKLPM5;*/

	  /*// Clock setup*/
	  /*CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers*/
	  /*CSCTL1 = DCOFSEL_0;                       // Set DCO to 1MHz*/
	  /*CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK; // Set ACLK = LFXTCLK; MCLK = DCO*/
	  /*CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers to 1*/
	  /*CSCTL4 &= ~LFXTOFF;						// Turn on LFXT*/

	  /*// Lock CS registers - Why isn't PUC created?*/
	  /*CSCTL0_H = 0;*/

	  /*// Enable interrupts*/
	  /*__bis_SR_register(GIE);*/

	  /*// Wait for button press*/
	  /*P4OUT |= BIT6;*/
	  /*while(P1IN & BIT1);*/
	  /*P4OUT &= ~BIT6;							// Turn off red LED*/

	// Mount the SD Card
	switch( f_mount(&sdVolume, "", 0) ){
		case FR_OK:
			status = 42;
			break;
		case FR_INVALID_DRIVE:
			status = 1;
			
			break;
		case FR_DISK_ERR:
			status = 2;
			break;
		case FR_NOT_READY:
			status = 3;
			break;
		case FR_NO_FILESYSTEM:
			status = 4;
			break;
		default:
			status = 5;
			break;
	}

	if(status != 42){
		// Error has occurred
		/*P4OUT |= BIT6;*/
		return status;
	}

//	DS3231GetCurrentTime();

	  char filename[] = "LOG2_00.csv";
	  FILINFO fno;
	  FRESULT fr;
	  uint8_t i;
	  for(i = 0; i < 100; i++){
		  filename[5] = i / 10 + '0';
		  filename[6] = i % 10 + '0';
		  fr = f_stat(filename, &fno);
		  if(fr == FR_OK){
			  continue;
		  }else if(fr == FR_NO_FILE){
			  break;
		  }else{
			  // Error occurred
			  return -2;
		  }
	  }

	// Initialize result variable


	// Open & write
	if(f_open(logfile, filename, FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {	// Open file - If nonexistent, create
		f_lseek(logfile, logfile->fsize);			// Move forward by filesize; logfile.fsize+1 is not needed in this application
		return 0;
		/*for(i = 0; i < 10; i++){*/
			/*f_printf(&logfile, "penis ", printValue[0], printValue[1]);*/
		/*}*/
		/*f_sync(&logfile);*/
		/*FloatToPrint(testFloat, printValue);*/
		/*for(i = 0; i < 10; i++){*/
			/*f_printf(&logfile, "penis2 ", printValue[0], printValue[1]);*/
		/*}*/
		/*f_close(&logfile);							// Close the file*/
		/*if (bw == 11) {*/
			/*P1OUT |= BIT0;*/
		/*}*/
	}
	return 7;
}
