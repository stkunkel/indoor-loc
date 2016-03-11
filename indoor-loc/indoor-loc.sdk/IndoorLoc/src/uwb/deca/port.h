/*
 * port.h
 *
 *  Created on: Feb 16, 2016
 *      Author: kugel
 */

#ifndef PORT_H_
#define PORT_H_

// need to put specific stuff here
#include "xparameters.h"
#include "xspi.h"

#define writetospi  writetospi_serial
#define readfromspi readfromspi_serial
void reset_DW1000(void);


#define port_GetEXT_IRQStatus()  0
#define port_DisableEXT_IRQ()
#define port_EnableEXT_IRQ()
#define port_CheckEXT_IRQ()

#endif /* PORT_H_ */
