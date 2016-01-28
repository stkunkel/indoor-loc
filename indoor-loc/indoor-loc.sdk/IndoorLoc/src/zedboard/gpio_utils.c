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
static XGpio gpio;
static u8 toggle = 0x00;
static u8 run = 0x01;

/*
 * Test LED Run
 */
void testLedRun() {
	//Variables
	int status;

	//Initialize
	status = initGpio();

	//Run
	while (1) {
		ledRun();
		sleep(1);
	}
}

/*
 * Test Toggle LED
 */
void testToggleLed() {
	//Variables
	int status;

	//Initialize
	status = initGpio();

	//Toggle
	while (1) {
		toggleLed(LED_MASK);
		sleep(1);
	}
}

/*
 * LED Run
 */
void ledRun() {
	//Get next LED configuration
	if (run == 0x80) {//If LED7 lights up, LED0 is next
		run = 0x01;
	} else { //Next LED is one to the left
		run = (run << 1);
	}

	//Go
	XGpio_DiscreteWrite(&gpio, LED_CHANNEL, run);
}

/*
 * Toggle LED
 */
void toggleLed(u8 ledMask) {
	toggle = (~toggle) & ledMask;
	XGpio_DiscreteWrite(&gpio, LED_CHANNEL, toggle);
}

/*
 * Initialize GPIO
 */
int initGpio() {
	//Variables
	int status;

	//Initialize GPIO
	status = XGpio_Initialize(&gpio, AXI_GPIO_DEVICE_ID);

	//Set Direction
	XGpio_SetDataDirection(&gpio, LED_CHANNEL, 0);

	//Clear LEDs
	if (status == XST_SUCCESS) {
		XGpio_DiscreteWrite(&gpio, LED_CHANNEL, 0);
	}

	return XST_SUCCESS;
}
