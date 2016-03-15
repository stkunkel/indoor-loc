/*
 * gpio_utils.c: Utilities for LEDs, etc.
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include "gpio_utils.h"

/*
 * Variables
 */
static XGpio inGpio, outGpio;
static u8 leds = 0;

/*
 * Get Switch Status
 */
u32 getSwitchStatus() {
	return (XGpio_DiscreteRead(&inGpio, SWITCH_CHANNEL));
}

/*
 * Get Button Status
 */
u32 getButtonStatus() {
	return (XGpio_DiscreteRead(&inGpio, BUTTON_CHANNEL));
}

/*
 * LED Toggle Test
 */
void testToggleLed() {
	u8 mask[] = { 0xFF, INT_MASK, IMU_MASK, ROBOT_MASK, UWB_MASK };
	int number = sizeof(mask);
	int i, j;

	for (i = 0; i < number; i++) {
		for (j = 0; j < 10; j++) {
			toggleLed(mask[i]);
			usleep(100000);
		}
	}

}

/*
 * Toggle LED
 * In: Bit Mask for LED(s)
 */
void toggleLed(u8 ledMask) {
	//LED is on
	if ((leds & ledMask) == ledMask) {
		leds = leds & (~ledMask); //clear LED
	} else { //LED is off
		leds = leds | ledMask; //enable LED
	}

	//Toggle
	XGpio_DiscreteWrite(&outGpio, LED_CHANNEL, leds);

}

/*
 * Clear All LEDs
 */
void clearLEDs() {
	//Clear LEDs
	leds = 0x00;
	XGpio_DiscreteWrite(&outGpio, LED_CHANNEL, leds);
}

/*
 * Initialize GPIO
 */
int initGpio() {
//Variables
	int status;

//Initialize GPIOs
	status = XGpio_Initialize(&outGpio, OUTGPIO_DEVICE_ID);
	status = XGpio_Initialize(&inGpio, INGPIO_DEVICE_ID);

//Set Direction
	XGpio_SetDataDirection(&outGpio, LED_CHANNEL, DIR_OUT);
	XGpio_SetDataDirection(&inGpio, 1, DIR_IN);
	XGpio_SetDataDirection(&inGpio, 2, DIR_IN);

//Clear LEDs
	if (status == XST_SUCCESS) {
		clearLEDs(status, &outGpio);
	}

	return XST_SUCCESS;
}
