/*
 * print_utils.c
 *
 *  Created on: Nov 27, 2015
 *      Author: ruppi
 */

#include "print_utils.h"

/*
 * Like printf
 */
int myprintf(const char *format, ...) {
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