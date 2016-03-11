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
#include "../imu/mpu.h"

/*
 * Defines
 */
#define INGPIO_DEVICE_ID	XPAR_AXI_GPIO_1_DEVICE_ID
#define OUTGPIO_DEVICE_ID	XPAR_AXI_GPIO_0_DEVICE_ID
#define LED_CHANNEL			1
#define SWITCH_CHANNEL		1
#define BUTTON_CHANNEL		2
#define DIR_IN				1
#define DIR_OUT				0
#define INT_MASK			0x10
#define IMU_MASK			0x20
#define ROBOT_MASK			0x40
#define UWB_MASK			0x80

/*
 * Functions
 */
u32 getSwitchStatus();
u32 getButtonStatus();
void testToggleLed();
void toggleLed(u8 ledMask);
void clearLEDs();
int initGpio();

#endif /* GPIO_UTILS_H_ */
