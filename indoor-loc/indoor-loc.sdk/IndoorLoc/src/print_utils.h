/*
 * print_utils.h
 *
 *  Created on: Nov 27, 2015
 *      Author: ruppi
 */

#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

//#define DEBUG //Comment for DEBUG prints, uncomment for print only for IMU viewer

int myprintf(const char *format, ...);

#endif /* PRINT_UTILS_H_ */
