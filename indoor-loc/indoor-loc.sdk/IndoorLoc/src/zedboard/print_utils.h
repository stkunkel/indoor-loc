/*
 * print_utils.h: Print utilities, i.e. Debug printing, XModem transmission
 * Author: Stephanie Kunkel
 */

#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include "xuartps.h"
#include "../program_parameters.h"

//Defines
#define XMODEM_PACKET_SIZE		133
#define XMODEM_PKG_CONT_SIZE	128
#define XMODEM_INDEX_HEAD		0
#define XMODEM_INDEX_CONTENT	3
#define XMODEM_INDEX_CRC		131


//Functions
void testXModem();
int xmodemTransmit(unsigned char *src, int srcsz);
void initXUartPs();
int myprintf(const char *format, ...);

#endif /* PRINT_UTILS_H_ */
