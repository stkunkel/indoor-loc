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

/*
 * Function Prototypes
 */
void toggleLed(u8 ledMask);

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
		toggleLed(LED);
		sleep(1);
	}
}

/*
 * Toggle LED
 */
void toggleLed(u8 ledMask){
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
