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
static int setAddr(u8* addr, char* addr_c);

/*
 * Functions
 */

/*
 * Wrapper to Set DMP Accel Bias
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetDmpAccelBias(long *bias) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = dmp_set_accel_bias(bias); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Set DMP Gyro Bias
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetDmpGyroBias(long *bias) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = dmp_set_gyro_bias(bias); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Get Fifo Count
 * Out: fifo count
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuGetFifoCnt(int* cnt) {
	//Variables
	int success, failCnt = 0;
	unsigned char* data = (unsigned char*) malloc(100 * sizeof(char));

	// Read Data
	do {
		// Execute Function
		success = imuI2cRead(imuAddr, 0x72, 2, data);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Convert
	*cnt = (data[0] << 8) | data[1];

	//Free memory and return
	free(data);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Enable DMP Gyro Calibration
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuEnableDmpGyroCal(unsigned char enable) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = dmp_enable_gyro_cal(enable); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Enable DMP Features
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuEnableDmpFeatures(unsigned short mask) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = dmp_enable_feature(mask); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Set FIFO Rate
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetFifoRate(unsigned short rate) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = dmp_set_fifo_rate(rate); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Set Sensors
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetSensors(unsigned char sensors) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_sensors(sensors);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Read Interrupt Pin Setup
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuReadIntConfig(unsigned char* config) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = imuI2cRead(0x68, 0x37, 2, config);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Setup Interrupt Pin
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuConfigureInt(unsigned char* config) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = imuI2cWrite(0x68, 0x37, 1, config);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Set Latched Interrupt
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetIntLatched(unsigned char enable) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_int_latched(enable);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Set Gyro Bias
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetGyroBias(long *gyro_bias) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_gyro_bias_reg(gyro_bias);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to set DMP State
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetDmpState(unsigned char enable) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_dmp_state(enable);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to set Gyro FSR
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetGyroFsr(unsigned short fsr) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_gyro_fsr(fsr);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to set Accel FSR
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetAccelFsr(unsigned char fsr) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_accel_fsr(fsr);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to reset FIFO
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuResetFifo() {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_reset_fifo();

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to set sample rate
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetSampleRate(unsigned short rate) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_sample_rate(rate);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to load DMP Firmware
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuDmpLoadMotionDriverFirmware() {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = dmp_load_motion_driver_firmware(); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to initialize MPU
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuInitMpu(struct int_param_s *int_param) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_init(int_param);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read From DMP FIFO
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuDmpReadFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = dmp_read_fifo(gyro, accel, quat, timestamp, sensors, more); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read Gyro Bias
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuReadGyroBias(long *gyro_bias) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_read_6500_gyro_bias(gyro_bias); //TODO

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Set Accel Bias
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuSetAccelBias(const long *accel_bias) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_set_accel_bias_6050_reg(accel_bias);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read Accel Bias
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuReadAccelBias(long *accel_bias) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_read_6050_accel_bias(accel_bias);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read Interrupt Status
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuGetIntStatus(short* status) {
	//Variables
	int success, failCnt = 0;

	// Read Data
	do {
		// Execute Function
		success = mpu_get_int_status(status);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (success != XST_SUCCESS);

	//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read Gyro Register
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuReadGyroReg(short *data, unsigned long *timestamp) {
//Variables
	int status, failCnt = 0;

// Read Data
	do {
		// Execute Function
		status = mpu_get_gyro_reg(data, timestamp);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (status != XST_SUCCESS);

//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read Accel Register
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuReadAccelReg(short *data, unsigned long *timestamp) {
//Variables
	int status, failCnt = 0;

// Read Data
	do {
		// Execute Function
		status = mpu_get_accel_reg(data, timestamp);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (status != XST_SUCCESS);

//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read Compass Register
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuReadCompassReg(short *data, unsigned long *timestamp) {
//Variables
	int status, failCnt = 0;

// Read Data
	do {
		// Execute Function
		status = mpu_get_compass_reg(data, timestamp);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (status != XST_SUCCESS);

//Return
	return XST_SUCCESS;
}

/*
 * Wrapper to Read Temperature
 * Returns 0 if successful and 21 if IIC bus is busy
 */
int imuReadTemp(long *data, unsigned long *timestamp) {
//Variables
	int status, failCnt = 0;

// Read Data
	do {
		// Execute Function
		status = mpu_get_temperature(data, timestamp);

		//Take care of unsuccessful IIC Reads
		failCnt++;
		if (failCnt > IIC_FAIL_MAX) {
			return XST_DEVICE_BUSY;
		}
	} while (status != XST_SUCCESS);

//Return
	return XST_SUCCESS;
}

/*
 * IIC Write
 */
int imuI2cWrite(unsigned char slave_addr, unsigned char reg_addr,
		unsigned char length, unsigned char *data) {
//Variables
	int status;

//IIC Write
	status = iic_burstWrite(&IicPs, slave_addr, reg_addr, length, data);

	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error on IIC Write (Addr: 0x%x, Err: 0x%x).\r\n",
				reg_addr, status);
		return status;
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
		myprintf("mpu_utils.c: Error on IIC Read (Addr: 0x%x, Err: 0x%x).\r\n",
				reg_addr, status);
		return status;
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
	*timestamp = getElapsedRuntimeUS() / 1000;
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
	imuAddr_cp = (char *) malloc(8 * sizeof(char));

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
