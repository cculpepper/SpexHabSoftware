/*
 * main.h
 *
 *  Created on: Sep 7, 2014
 *      Author: Nipun Gunawardena
 */

#include "./FatFS/ff.h"
#ifndef SD_H_
#define SD_H_
int SDOpenFile ( FIL *logfile);
#define MCLK_FREQUENCY 1000000		// Used in mmc.c but is project specific

#endif /* SD_H_ */
