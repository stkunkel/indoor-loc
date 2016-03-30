/*
 * run.c: Main Program
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "platform.h"
#include "imu/mpu.h"
#include "imu/mpu_int.h"
#include "robot/pwmsw.h"
#include "uwb/uwb.h"
#include "uwb/decaApp/appCfg.h"
#include "zedboard/print_utils.h"
#include "zedboard/gpio_utils.h"
#include "zedboard/timer_int.h"
#include "bool.h"
#include "program_parameters.h"

/*
 * Defines
 */
#define DATA_NO_DMP_RUNS 	10000
#define DATA_WITH_DMP_RUNS 	0
#define DISPLAY_RUNS		0
#define SEPARATOR			" "

/*
 * Function Prototypes
 */
int moveAndCollect(unsigned int sampleTime, Joint joint, float* angle,
		unsigned int numOfAngles, bool collect);
int printGeneric(short int printMask, char* separator,
		unsigned int numberOfRuns);


/*
 * Main
 */
int main() {
	//Variables
	float angle[6];
	unsigned int numOfAngles;

	//Set angles
	angle[0] = 0.0;
	angle[1] = 30.0;
	angle[2] = 60.0;
	angle[3] = 90.0;
	angle[4] = 60.0;
	angle[5] = 30.0;

	//Compute number of angles
	numOfAngles = sizeof(angle) / sizeof(angle[0]);

	//Init Platform
	init_platform();

	//Print Output Separator
	myprintf(".........Program Start...........\n\r");

	//Init GPIO (for Buttons, Switches and LEDs)
	initGpio();

	//Reset Robot
	reset();

//	Generic Print to Serial Port
//	printGeneric(PRINT_FOR_VIEWER, SEPARATOR, 10);

//Move Robot and collect all sensor data (IMU and UWB)
	moveAndCollect(60, wrist, angle, numOfAngles, BOOL_FALSE);

//Clear LEDs
	clearLEDs();

//Stay in here
	while (1) {
		;
	}

	//Finish
	cleanup_platform();
	return 0;
}

/*
 * Move Robot and Collect IMU and UWB Sensor Data
 * For Online Computation, make sure FIFO Frequency is not higher than 200Hz
 */
int moveAndCollect(unsigned int sampleTime, Joint joint, float* angle,
		unsigned int numOfAngles, bool collect) {
	//Variables
	int status;
	SensorPwmData data;
	uint32_t cnt = 0, samples = 0, angleid = 0, anglecnt = 0, button, sw;
	double dist_temp = 0.0;


	//Compute number of data samples
	samples = sampleTime * FIFO_RATE;

	//1. Initialize
	//Robot
	status = reset();
	if (status != PWM_SUCCESS) {
		return PWM_FAILURE;
	}

	//Initialize UWB
	status = initUwb();
	if (status != DWT_SUCCESS) {
		return DWT_ERROR;
	}

	//Initialize IMU and Timer Interrupt
	status = initIMU(CAL_TIME);
	if (status != XST_SUCCESS) {
		printf("Could not initialize IMU.\r\n");
		return PWM_FAILURE;
	}

	//Get Switches
	sw = getSwitchStatus();

	//Time to calibrate UWB if sw1 = 1
	while ((sw & 0x7) == 1) {
		if (needToUpdateData()) {

			//Get Distance
			status = uwbGetDistance(&dist_temp);
			if (status == DWT_SUCCESS) {
				printf("Distance: %f\r\n", dist_temp);
				deca_sleep(RNG_DELAY_MS);
			}

			//Check for buttons
			button = getButtonStatus();

			//If buttons pressed, in-/decrease antenna delay
			if (button & 1) { //BTNC
				decreaseAntennaDelay();
			} else if (button & 2) { //BTND
				increaseAntennaDelay();
			}
		}

		//Get Switches
		sw = getSwitchStatus();
	}

	//Reset Counter
	cnt = 0;

	//Print
	printf("Start to collect data now.\r\n");

	//Get Samples
	while (cnt < samples) {

		//Get Sensor Data
		while (needToUpdateData() == BOOL_FALSE) {
			usleep(1);
		}

		// Move Robot
		if (cnt % ((FIFO_RATE * HSS_422_TIME_FOR_90_DGRS) / 1000) == 0) {

			//Increase Angle Counter
			anglecnt++;

			//Check whether new angle should be set
			if (anglecnt >= STEPS_BEFORE_NEW_ANGLE) {

				//Reset Angle Counter
				anglecnt = 0;

				//Reset Angle ID
				if (angleid >= numOfAngles) {
					angleid = 0;
				}

				//Set angle and increase ID
				setAngle(joint, angle[angleid]); //angleid
				angleid++;

				//Toggle LED
				toggleLed(ROBOT_MASK);
			}
		}

		//Get PWM Data
		getPwmRegValues(data.pwmValues);

		//Read UWB Value
		status = uwbGetDistance(&dist_temp);
		if (status == DWT_SUCCESS) {
			data.uwb_dist_cm = (int16_t) (dist_temp * 100);
		}

		//Read Sensor Data and write to memory
		status = readFromRegs(data.mpuData.gyro, data.mpuData.accel,
				data.mpuData.compass, &data.mpuData.temp, 0, SENSORS_ALL);

		//Read successful?
		if (status == XST_SUCCESS) {
			//LED Run
			if (cnt % FIFO_RATE == 0) {
				toggleLed(IMU_MASK);
			}

			//Collect Data in required
			if (collect == BOOL_TRUE) {

				//Store Values in buffer
				storeInBuff(&data);

			} else {
				//Update Data
				status = updateData();

				//Check whether data should be printed
				if (status == XST_SUCCESS) {
					if ((cnt % (FIFO_RATE / IMUVIEWER_FREQ)) == 0) {

						//Print
						status = printforDisplay(PRINT_FOR_VIEWER, " ");

						//Print new line
						if (status == XST_SUCCESS) {

							//Print new line
							printf("\n\r");
						}
					}
				}
			}

			//Increase Counter
			cnt++;

		} else if (status == XST_DEVICE_BUSY) { //IIC bus is busy
			break;
		}
	}

	//Reset Robot
	reset();

	//Transmit Buff
	if (collect == BOOL_TRUE) {
		status = transmitBuf();
	}

	//Return
	return status;
}

/*
 * Prints Quaternions and/or Position for IMU Viewer
 * In: print mask, separator between sensors, number of runs (if 0 --> print forever)
 * Returns 0 if successful
 */
int printGeneric(short int printMask, char* separator,
		unsigned int numberOfRuns) {
	//Variables
	int cnt = 0, printcnt = 0, status;
	bool endless = BOOL_FALSE;

	//Set number of runs, if endless print
	if (numberOfRuns == 0) {
		endless = BOOL_TRUE;
		numberOfRuns = 1;
	}

	//Init IMU and calibrate if specified
	status = initIMU(CAL_TIME);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Print to Serial Port
	myprintf(".........Print to UART...........\n\r");
	while (printcnt < numberOfRuns) {
		//Increase Counter
		cnt++;

		//Reset Status
		status = XST_SUCCESS;

		//Wait for Interrupt Interrupt
		if (needToUpdateData() == BOOL_TRUE) {
			//Update Data
			status = updateData();

			//Check whether data should be printed
			if (status == XST_SUCCESS) {
				if ((cnt % (FIFO_RATE / IMUVIEWER_FREQ)) == 0) {

					//Print
					status = printforDisplay(printMask, separator);

					//Print new line
					if (status == XST_SUCCESS) {
						//Increase print counter
						printcnt++;

						//Print new line
						if (printMask != PRINT_NOTHING) {
							printf("\n\r");
						}
					} else {
						printcnt--;
					}

					//For endless prints
					if (endless == BOOL_TRUE) {
						printcnt = 0;
					}

				}
			} else if (status == XST_DEVICE_BUSY) {	//IIC bus is busy
				return status;
			} else {	//Other error
				cnt--;
			}
		} else { // No data available
			cnt--;
		}
	}

//Return
	return status;
}
