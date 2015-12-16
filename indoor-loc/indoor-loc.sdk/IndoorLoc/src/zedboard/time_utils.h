/*
 * time_utils.h: Convert XTime to us or s; get elapsed runtime in us or s
 * Author: Stephanie Kunkel
 */

#ifndef TIME_UTILS_H_
#define TIME_UTILS_H_

//Includes
#include "xtime_l.h"

//Defines
#define COUNTS_PER_USECOND  	(COUNTS_PER_SECOND / 1000000U)

//Functions
unsigned long getElapsedRuntimeS();
unsigned long getElapsedRuntimeUS();
unsigned long xtimeToSec(XTime counts);
unsigned long xtimeToUSec(XTime counts);

#endif /* TIME_UTILS_H_ */
