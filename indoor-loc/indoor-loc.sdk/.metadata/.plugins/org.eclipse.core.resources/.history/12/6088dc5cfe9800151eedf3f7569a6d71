/*
 * mpu_utils.c
 *
 *  Created on: Oct 28, 2015
 *      Author: rupprich
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
 * Write Two Registers --> 16 Bit Data
 */
int writeDoubleReg(u8 reg_h, u8 reg_l, u16* data) {
	int status;
	u8 data8;

	//write high bits
	data8 = (*data & 0xFF00) >> 8; //get high bites
	status = imuI2cWrite(imuAddr, reg_h, sizeof(data8), &data8);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf(
				"mpu_utils.c: Error during reading double register (high bits of 0x%x).\r\n",
				reg_h);
#endif
		return XST_FAILURE;
	}

	//write low bits
	data8 = *data & 0x00FF; //get low bites
	status = imuI2cWrite(imuAddr, reg_l, sizeof(data8), &data8);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf(
				"mpu_utils.c: Error during reading double register (low bits of 0x%x).\r\n",
				reg_l);
#endif
		return XST_FAILURE;
	}

	return 0;
}

/*
 * Write to Single Register
 */
int writeSingleReg(u8 reg, u8* data) {
	int status;
	status = imuI2cWrite(imuAddr, reg, sizeof(*data), data);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf(
				"mpu_utils.c: Error during writing single register (0x%x).\r\n",
				reg);
#endif
		return XST_FAILURE;
	}
	return 0;
}

/*
 * Read Two Registers --> 16 Bit Data
 */
int readDoubleReg(u8 reg_h, u8 reg_l, u16* data) {
	int status;
	u8 data8;

	//get high bits
	status = imuI2cRead(imuAddr, reg_h, sizeof(data8), &data8);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf(
				"mpu_utils.c: Error during reading double register (high bits of 0x%x).\r\n",
				reg_h);
#endif
		return XST_FAILURE;
	}

	*data = data8 << 8;

	//get low bits
	status = imuI2cRead(imuAddr, reg_l, sizeof(data8), &data8);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf(
				"mpu_utils.c: Error during reading double register (low bits of 0x%x).\r\n",
				reg_l);
#endif
		return XST_FAILURE;
	}

	*data = *data | data8;

	return 0;
}

/*
 * Read from Single Register
 */
int readSingleReg(u8 reg, u8* data) {
	int status;
	status = imuI2cRead(imuAddr, reg, sizeof(*data), data);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf(
				"mpu_utils.c: Error during reading single register (0x%x).\r\n",
				reg);
#endif
		return XST_FAILURE;
	}
	return 0;
}

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
#ifdef DEBUG
		xil_printf("mpu_utils.c: Error on IIC Write (0x%x).\r\n", reg_addr);
#endif
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
#ifdef DEBUG
		xil_printf("mpu_utils.c: Error on IIC Read (0x%x).\r\n", reg_addr);
#endif
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
unsigned long imuGet_ms() {
	XTime time; //use global timer in Zynq SOC --> increases every 2 cycles
	XTime_GetTime(&time);
	return time;
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
#ifdef DEBUG
		xil_printf("mpu_utils.c: Error initializing IIC.\r\n");
#endif
		return XST_FAILURE;
	}

	//Set IMU Address
	status = setAddr(imuAddr, imuAddr_cp);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf("mpu_utils.c: Error setting IMU address.\r\n");
#endif
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
#ifdef DEBUG
		xil_printf("mpu_utils.c: Error initializing GPIO Config.\r\n");
#endif
		return XST_FAILURE;
	}

	//2. Set ad0
	status = writeAD0(&addr_last_bit);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		xil_printf("mpu_utils.c: Error setting AD0 for IMU.\r\n");
#endif
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
