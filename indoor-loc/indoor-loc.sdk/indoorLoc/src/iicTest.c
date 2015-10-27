/*
 * Includes
 */
#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xiicps.h"
#include "iic_utils.h"

/*
 * Main Program
 */
/*int main() {
	//Variables
	int Status;
	volatile XIicPs IicPs;

	//Output
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	print("\n\r---Entering main---\n\r");

	//IIC Init
	Status = iic_init(&IicPs, IIC_DEVICE_ID, IIC_SCLK_RATE);
	if (Status != XST_SUCCESS) {
		xil_printf("I2C Initialization failed.\r\n");
	} else {
		xil_printf("I2C Initialization successful.\r\n");
	}

	return XST_SUCCESS;
}*/
