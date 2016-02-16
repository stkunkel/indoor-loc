/*
 * print_utils.c: Print utilities for debug prints
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include "print_utils.h"

/*
 * Global Variables
 */
XUartPs hUart;

/*
 * Test XModem Transmission
 */
void testXModem() {
#define XMSIZE 1000000
	//Variables
	int status;

	//Init
	initXUartPs();

	//Initialize Buffer
	static unsigned char tbuf[XMSIZE];
	int i;
	for (i = 0; i < XMSIZE; i++) {
		tbuf[i] = i ^ 0x12;
	}

	//Transmit
	printf("XMODEM TEST: Start your receiver!\r\n");
	status = xmodemTransmit(tbuf, XMSIZE);
	printf("XMODEM test finished with status %d.\r\n", status);
}

/*
 * Initialize XUartPs
 */
void initXUartPs() {
	XUartPs_CfgInitialize(&hUart,
			XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID),
			XPAR_XUARTPS_0_BASEADDR);
	XUartPs_SetBaudRate(&hUart, XUARTPS_DFT_BAUDRATE);
}

/*
 * Like printf
 * Prints only if DEBUG is defined in print_utils.h
 */
int myprintf(const char *format, ...) { //BUG: does only work for full lines, otherwise: delayed print
	int done = 0;
#ifdef DEBUG
	//printf("Error: ");
	va_list arg;
	va_start(arg, format);
	done = vfprintf(stdout, format, arg);
	va_end(arg);
#endif
	return done;
}
