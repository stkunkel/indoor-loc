/*
 * print_utils.h: Print utilities, i.e. Debug printing
 * Author: Stephanie Kunkel
 */

#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

//#define DEBUG //UNComment for DEBUG prints, COMMENT for print only for IMU viewer

int myprintf(const char *format, ...);

#endif /* PRINT_UTILS_H_ */
