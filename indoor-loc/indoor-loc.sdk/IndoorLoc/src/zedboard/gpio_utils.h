/*
 * gpio_utils.h
 * Author: Stephanie Kunkel
 */

#ifndef GPIO_UTILS_H_
#define GPIO_UTILS_H_

/*
 * Includes
 */
#include <unistd.h>
#include "xparameters.h"
#include "xstatus.h"
#include "xgpio.h"

/*
 * Defines
 */
#define AXI_GPIO_DEVICE_ID	XPAR_AXI_GPIO_0_DEVICE_ID
#define LED_CHANNEL			2
#define LED_MASK			0x55

/*
 * Functions
 */
void testLedRun();
void testToggleLed();
void ledRun();
void toggleLed(u8 ledMask);
int initGpio();

#endif /* GPIO_UTILS_H_ */
