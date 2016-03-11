/*
 * dataBuffer.c: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include "dataBuffer.h"

/*
 * Global Variables
 */
static uint32_t cnt = 0;
static unsigned char* bufCurr = (unsigned char*) BUF_ADDR + sizeof(cnt);

/*
 * Store in Buffer
 * In: Pointer to data set
 */
void storeInBuff(SensorPwmData* data) {
	//Check for buffer overflow
	if (bufCurr > (unsigned char*) BUF_MAX) {
		transmitBuf();
		cnt = 0;
		bufCurr = (unsigned char*) BUF_ADDR + sizeof(cnt);
	}

	//Store PWM Values in buffer
	*bufCurr = (unsigned char) (data->pwmValues[0] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[0] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[0] & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[0] & BYTE3) >> 24);
	bufCurr++;
	*bufCurr = (unsigned char) (data->pwmValues[1] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[1] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[1] & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[1] & BYTE3) >> 24);
	bufCurr++;
	*bufCurr = (unsigned char) (data->pwmValues[1] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[2] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[2] & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[2] & BYTE3) >> 24);
	bufCurr++;
	*bufCurr = (unsigned char) (data->pwmValues[3] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[3] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[3] & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[3] & BYTE3) >> 24);
	bufCurr++;
	*bufCurr = (unsigned char) (data->pwmValues[4] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[4] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[4] & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[4] & BYTE3) >> 24);
	bufCurr++;
	*bufCurr = (unsigned char) (data->pwmValues[5] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[5] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[5] & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->pwmValues[5] & BYTE3) >> 24);
	bufCurr++;

	//Store MPU Data in buffer
	*bufCurr = (unsigned char) (data->mpuData.gyro[0] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.gyro[0] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.gyro[1] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.gyro[1] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.gyro[2] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.gyro[2] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.accel[0] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.accel[0] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.accel[1] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.accel[1] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.accel[2] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.accel[2] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.compass[0] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.compass[0] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.compass[1] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.compass[1] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.compass[2] & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.compass[2] & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) (data->mpuData.temp & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.temp & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.temp & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->mpuData.temp & BYTE3) >> 24);
	bufCurr++;

	//UWB Distance
	*bufCurr = (unsigned char) (data->uwb_dist & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((data->uwb_dist) & BYTE1 >> 8);
	bufCurr++;

	//Increase Counter
	cnt++;
}

/*
 * Transmit Buffer
 * Disables Timer Interrupt during Transmission
 * Returns 0 if done
 */
int transmitBuf() {
	//Variables
	unsigned char* bufStart;

	//Initialize bufStart
	bufStart = (unsigned char*) BUF_ADDR;

	//Write number of samples into buffer
	*bufStart = (unsigned char) (cnt & BYTE0);
	bufStart++;
	*bufStart = (unsigned char) ((cnt & BYTE1) >> 8);
	bufStart++;
	*bufStart = (unsigned char) ((cnt & BYTE2) >> 16);
	bufStart++;
	*bufStart = (unsigned char) ((cnt & BYTE3) >> 24);

	//Disable Timer Interrupts
	disableTmrInt();

	//Initialize XUart
	initXUartPs();

	//Transmit buf
	//printf("XModem Transmission starts.\r\n");
	xmodemTransmit((unsigned char*) BUF_ADDR,
			(sizeof(cnt) + cnt * DATA_NUMBER_OF_BYTES));
	//printf("XModem Transmission finished.\r\n");

	//Enable Timer Interrupts
	reenableTmrInt();

	//Return
	return 0;
}

/*
 * Reset Buff Address
 * I.e. Discard all samples taken
 */
void resetBuff() {
	bufCurr = (unsigned char*) BUF_ADDR;
	cnt = 0;
}