/*
 * uwb.h:UWB Control
 *
 *  Created on: Mar 11, 2016
 *      Author: rupprich
 */

#ifndef UWB_H_
#define UWB_H_

// Includes
#include <stdio.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xspi.h"
#include "decadriver/deca_device_api.h"
#include "decadriver/deca_regs.h"
#include "deca/deca_sleep.h"
#include "deca/deca_spi.h"
#include "deca/port.h"
#include "../zedboard/print_utils.h"


/*
 * Enum
 */
typedef enum { send, receive, wait } UwbState;

/*
 * Function Prototypes
 */
double distToDouble(int16_t distance);
int16_t distToInt(double distance);
int getDistance(double* p_distance);
int ssTwrInit(void);
int transmit();
int prepareSsTwrInit();
int waitAfterReceive();
void increaseAntennaDelay();
void decreaseAntennaDelay();
int uwbGetDistance(double* p_distance);
int initUwb();
void reset_DW1000();


#endif /* UWB_H_ */