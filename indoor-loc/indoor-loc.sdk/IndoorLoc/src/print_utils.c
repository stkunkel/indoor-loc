/*
 * print_utils.c: Print utilities for debug prints
 * Author: Stephanie Kunkel
 */

#include "print_utils.h"

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
