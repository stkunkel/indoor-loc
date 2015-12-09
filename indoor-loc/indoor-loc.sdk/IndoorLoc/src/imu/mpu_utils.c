/*
 * mpu_utils.c: Utilities for communication with MPU9150
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include "mpu_utils.h"

/*
 * Global Variables
 */
XGpioPs Gpio; /* The driver instance for GPIO Device. */
XIicPs IicPs; /* The driver instance for IIC Device. */
static u8 imuAddr; //IMU addr
static char *imuAddr_cp;

/*
 * Function Prototypes
 */
static int writeAD0(u32 *data);
static int readInt(u32 *DataRead);
static int setAddr(u8* addr, char* addr_c);

/*
 * Functions
 */


/*
 * IIC Write
 */
int imuI2cWrite(unsigned char slave_addr, unsigned char reg_addr,
		unsigned char length, unsigned char *data) {
	//Variables
	int status;

	//IIC Read
	status = iic_burstWrite(&IicPs, slave_addr, reg_addr, length, data);

	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error on IIC Write (0x%x).\r\n", reg_addr);
		return XST_FAILURE;
	}

	//Return
	return XST_SUCCESS;
}

/*
 * IIC Read
 */
int imuI2cRead(unsigned char slave_addr, unsigned char reg_addr,
		unsigned char length, unsigned char *data) {
	//Variables
	int status;

	//IIC Read
	status = iic_read2(&IicPs, slave_addr, reg_addr, data, length);

	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error on IIC Read (0x%x).\r\n", reg_addr);
		return XST_FAILURE;
	}

	//Return
	return XST_SUCCESS;
}

/*
 * Delay
 */
void imuDelay(unsigned long ms) {
	usleep(ms * 1000);
}

/*
 * Get Timestamp
 */
void imuGet_ms(unsigned long* timestamp) {
	XTime counts; //use global timer in Zynq SOC --> increases every 2 cycles
	XTime_GetTime(&counts);
	*timestamp = (unsigned long) (counts / COUNTS_PER_SECOND);
}

/*
 * Log I
 */
int imuLog_i(const char *format, ...) {
	int done = 0;
	//printf("Error: ");
	va_list arg;
	va_start(arg, format);
	done = myprintf(format, arg);
	va_end(arg);
	return done;
}

/*
 * Log I
 */
int imuLog_e(const char *format, ...) {
	int done = 0;
	//printf("Error: ");
	va_list arg;
	va_start(arg, format);
	done = myprintf(format, arg);
	va_end(arg);
	return done;
}

/*
 * Initialize IMU
 */
int imuInit(u8 *imuAddr) {
	//Variables
	int status;
	imuAddr_cp = (char *) malloc(8);

	//Abort old IIC transaction
	status = iic_reset_init_abort(&IicPs, IIC_DEVICE_ID, IIC_SCLK_RATE);

	//Init IIC
	status = iic_init(&IicPs, IIC_DEVICE_ID, IIC_SCLK_RATE);
	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error initializing IIC.\r\n");
		return XST_FAILURE;
	}

	//Set IMU Address
	status = setAddr(imuAddr, imuAddr_cp);
	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error setting IMU address.\r\n");
		return XST_FAILURE;
	}

	//Return
	return XST_SUCCESS;
}

/*
 * Set IMU Address
 */
int setAddr(u8* addr, char* addr_c) {
	//1. Initialize GPIO
	int status;
	u32 addr_last_bit = IMU_LAST_ADDR_BIT;
	XGpioPs_Config *ConfigPtr;

	/* Initialize the GPIO driver. */
	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);
	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error initializing GPIO Config.\r\n");
		return XST_FAILURE;
	}

	//2. Set ad0
	status = writeAD0(&addr_last_bit);
	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error setting AD0 for IMU.\r\n");

		return XST_FAILURE;
	}

	//3. Set IMU address
	imuAddr = IMU_ADDR_MASK | IMU_LAST_ADDR_BIT;
	*addr = imuAddr;
	*imuAddr_cp = (char) *addr;
	//sprintf(imuAddr_cp, "%d", *addr);

	//Return
	return XST_SUCCESS;
}

/*
 * Write to Pin
 */
int writeAD0(u32 *data) {
	/*
	 * Set the direction for the pin to be output and
	 * Enable the Output enable for the LED Pin.
	 */
	XGpioPs_SetDirectionPin(&Gpio, PS_IMU_AD0_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, PS_IMU_AD0_PIN, 1);

	/* Set the GPIO output to be low. */
	XGpioPs_WritePin(&Gpio, PS_IMU_AD0_PIN, *data);

	return XST_SUCCESS;
}

/*
 * Get Interrupt
 */
int readInt(u32 *DataRead) {
	/* Set the direction for the specified pin to be input. */
	XGpioPs_SetDirectionPin(&Gpio, PS_IMU_INT_PIN, 0x0);

	/* Read the state of the data so that it can be  verified. */
	*DataRead = XGpioPs_ReadPin(&Gpio, PS_IMU_INT_PIN);

	return XST_SUCCESS;
}
