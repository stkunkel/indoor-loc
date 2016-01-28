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
static u32 toggle = 0x00000000;

/*
 * Function Prototypes
 */
void toggleLed();

/*
 * Test Toggle LED
 */
void testToggleLed(){
	//Variables
	int status;

	//Initialize
	status = initGpio();

	//Toggle
	while (1){
		toggleLed();
		sleep(1);
	}
}

/*
 * Toggle LED
 */
void toggleLed(){
	toggle = (~toggle) & LED;
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
