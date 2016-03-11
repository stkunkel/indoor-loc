/*
 * uwb.c: UWB Control
 * Author: Stephanie Kunkel
 */

#include "uwb.h"

/*
 * Global Variables
 */
XGpio uwbGpio;
XSpi decaSpi;
UwbState state = send;

/*
 * Convert Distance to double
 * In: distance as int16_t
 * Returns distance as double
 */
double distToDouble(int16_t distance){
	return ((double) distance / UWB_DIST_SENS);
}

/*
 * Convert Distance to 16-bit integer
 * In: distance
 * Returns distance as int16_t
 */
int16_t distToInt(double distance){
	return ((int16_t) (distance * UWB_DIST_SENS));
}

/*
 * State Machine to get Distance
 * In: Pointer to distance variable
 * Returns 0 if distance has been computed, -1 if process is still in progress
 */
int uwbGetDistance(double* p_distance) {
	//Variables
	int status;

	//State Machine
	switch (state) {
	case send:
		status = transmit();
		if (status == DWT_SUCCESS) {
			state = receive;
		}
		break;

	case receive:
		status = getDistance(p_distance);
		if (status == DWT_SUCCESS) {
			//Return Success
			state = wait;
			return DWT_SUCCESS;

		} else if (status == DWT_ERROR) {
			state = send;
		}
		break;

	case wait:
		if (waitAfterReceive() <= 0) {
			state = send;
		}
		break;

	default:
		break;
	}

	//Return
	return DWT_ERROR;
}

/*
 * Initialize UWB
 */
int initUwb() {
	//Variables
	int status;

	//Initialize SPI
	status = openspi();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Reset and Prepare DWM1000 Module
	status = prepareSsTwrInit();
	if (status != DWT_SUCCESS) {
		return status;
	}

	//Reset State Machine
	state = send;

	//Return
	return DWT_SUCCESS;
}

/*
 * Reset DW1000
 */
void reset_DW1000() {
	//Variables
	u32 uwbIn;

	//Initialize UWB GPIO
	XGpio_Initialize(&uwbGpio, XPAR_AXI_GPIO_UWB_DEVICE_ID);

	//Reset On
	XGpio_DiscreteWrite(&uwbGpio, 1, 0); // reset on
	uwbIn = XGpio_DiscreteRead(&uwbGpio, 2);

	//Reset Off
	XGpio_DiscreteWrite(&uwbGpio, 1, 0xff); // reset off
	uwbIn = XGpio_DiscreteRead(&uwbGpio, 2);
	while (0 == (uwbIn & 0x4)) {
		uwbIn = XGpio_DiscreteRead(&uwbGpio, 2);
	}
	myprintf("UBW reset completed\r\n");

	deca_sleep(2);
}
