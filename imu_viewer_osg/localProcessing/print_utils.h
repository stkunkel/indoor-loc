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
#include "program_parameters.h"

//Defines
#define XMODEM_PACKET_SIZE	1030
#define XMODEM_PKG_CONT_SIZE	1024

//Functions
int myprintf(const char *format, ...);

#endif /* PRINT_UTILS_H_ */
