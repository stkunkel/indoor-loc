/*
 * print_utils.h: Print utilities, i.e. Debug printing
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

//Functions
int xmodemTransmit(unsigned char *src, int srcsz);
int myprintf(const char *format, ...);

#endif /* PRINT_UTILS_H_ */
