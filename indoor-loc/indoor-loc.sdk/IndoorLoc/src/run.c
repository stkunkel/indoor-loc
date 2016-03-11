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
//#include "robot/pwmsw.h"
#include "robot/robotCtrl.h"
#include "uwb/uwb.h"
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
int moveAndCollect(unsigned int sampleTime, bool collect);
int printGeneric(short int printMask, char* separator,
		unsigned int numberOfRuns);
int printForImuViewer(short int printMask, char* separator,
		unsigned int numberOfRuns);
int printDataUsingDMP(short sensors, bool initialCalibration,
		bool dmpCalibration, char* separator, unsigned int numberOfRuns);
int printDataWithoutDMP(short sensors, char* separator,
		unsigned int numberOfRuns);

/*
 * Main
 */
int main() {
	//Init Platform
	init_platform();

	//Print Output Separator
	myprintf(".........Program Start...........\n\r");

	//Init GPIO (for Buttons, Switches and LEDs)
	initGpio();

	//Reset Robot
	reset();

	//Print Data without DMP
//	printDataWithoutDMP(SENSORS_ALL, SEPARATOR, DATA_NO_DMP_RUNS);

//Print Data with DMP without initial or DMP gyro calibration
//	printDataUsingDMP(SENSORS_ALL, 0, 0, SEPARATOR, DATA_WITH_DMP_RUNS);

//Print Data with DMP with initial calibration but no DMP gyro calibration
//	printDataUsingDMP(SENSORS_ALL, 1, 0, SEPARATOR, DATA_WITH_DMP_RUNS);

//Print Data with DMP with and DMP gyro calibration
//	printDataUsingDMP(SENSORS_ALL, 1, 1, SEPARATOR, DATA_WITH_DMP_RUNS);

//Print Quaternions and Position to Serial Port
//	printForImuViewer(PRINT_ALL, SEPARATOR, DISPLAY_RUNS);

//	Generic Print to Serial Port
//	printGeneric(PRINT_SENSORS, SEPARATOR, DISPLAY_RUNS);

//Quaternion Drift
//printQuaternionDriftAfterXMin(QUAT_DRIFT_MIN);

//PWM Test
//	pwmTest();

//	//Timer Test
//	timerTest(FIFO_RATE);

//Test position update functionality
//	testPositionUpdate();

//Test Matrix Inverse
//	testMatrixInverse();

//Test Quaternions for IMU Viewer
//	quaternionTest();

//Test Quaternion Computation
//	testQuaternionComputation();

//Test Vector Rotation
//	testVectorRotation();

//Test XModem
//	testXModem();

//Test Euler Angles
//	testEulerAngles();

//Test Gyro Sens Computation
//	testMpuGyroSensCalc();

//Collect Data
//	collectRegisterData(1800, 0); //1min sampling, no calibration

//Collect Robot Movement Data
//	int i = 0;
//	for (i = 2; i < 15; i++) {
//	collectRobotMvmtData(10, 0, BOOL_TRUE);
//	}

	//Test LED Toggle
//	testToggleLed();

	//Move Robot and collect all sensor data (IMU and UWB)
	moveAndCollect(10, BOOL_TRUE);

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
 */
int moveAndCollect(unsigned int sampleTime, bool collect) {
	//Variables
	int status;
	SensorPwmData data;
	uint32_t cnt = 0, samples = 0, angleid = 0, anglecnt = 0;
	Joint joint = wrist;
	float angle[2];
	double dist_temp = 0.0;

	//Set angles
	angle[0] = 0.0;
	angle[1] = 90.0;

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

	//Reset Counter
	cnt = 0;

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
			if (anglecnt >= 10) {

				//Reset Angle Counter
				anglecnt = 0;

				//Reset Angle ID
				if (angleid > 2) {
					angleid = 0;
				}

				//Set angle and increase ID
				setAngle(joint, angle[angleid]); //angleid
				toggleLed(ROBOT_MASK);
				angleid++;
			}
		}

		//Get PWM Data
		getPwmRegValues(data.pwmValues);

		//Read UWB Value
		status = uwbGetDistance(&dist_temp);
		if (status == DWT_SUCCESS) {
			data.uwb_dist_cm = (int16_t)(dist_temp * 100);
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

		} else if (status == XST_DEVICE_BUSY) {
			break;
		}
	}

	//Reset Robot
	reset();

	//Transmit Buff
	status = transmitBuf();

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
			} else if (status == XST_DEVICE_BUSY) {
				return status;
			} else {
				cnt--;
			}
		} else {
			cnt--;
		}
	}

//Return
	return status;
}

/*
 * Prints Quaternions and/or Position for IMU Viewer
 * In: print mask, separator between sensors, number of runs (if 0 --> print forever)
 * Returns 0 if successful
 */
int printForImuViewer(short int printMask, char* separator,
		unsigned int numberOfRuns) {
	//Variables
	int cnt = 0, printcnt = 0, status;
	bool endless = BOOL_FALSE;

	//Set number of runs, if endless print
	if (numberOfRuns == 0) {
		numberOfRuns = IMUVIEWER_FREQ;
		endless = BOOL_TRUE;
	}

	//Init
	myprintf(".........Configure MPU and DMP...........\n\r");
	status = configureDMP(FEATURES_CAL);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Calibrate
	myprintf(".........Calibrate...........\n\r");
	status = calibrateGyrAcc(CAL_SAMPLES);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Enable Interrupts
	status = setupDMPInt();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Adjust Number of Runs
	numberOfRuns *= (FIFO_RATE / IMUVIEWER_FREQ);

	//Print Quaternions to Serial Port
	myprintf(".........Print for IMU Viewer...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Reset Status
		status = XST_SUCCESS;

		//Wait for Interrupt Interrupt
		if (dmpDataAvailable() == BOOL_TRUE) {
			//Update Data
			status = updateData();

			//Check whether data should be printed
			if (status == XST_SUCCESS) {
				//Increase print counter
				printcnt++;

				if ((printcnt % (FIFO_RATE / IMUVIEWER_FREQ)) == 0) {
					//Reset printcnt
					printcnt = 0;

					//Print
					status = printforDisplay(printMask, separator);

					//Print new line
					if (status == XST_SUCCESS) {
						printf("\n\r");
					} else {
						cnt--;
					}

					//For endless prints
					if (endless == BOOL_TRUE) {
						cnt = 0;
					}

				}
			} else if (status == XST_DEVICE_BUSY) {
				return status;
			} else {
				cnt--;
				printcnt--;
			}
		} else {
			cnt--;
		}

		//Make sure only successful prints count
		if (numberOfRuns < 1) {
			cnt = 0;
		}
	}

//Return
	return status;
}

/*
 * Print Data using DMP
 * In: bool for initial calibration and dmp calibration, number of runs (if 0 --> endless loop)
 */
int printDataUsingDMP(short sensors, bool initialCalibration,
		bool dmpCalibration, char* separator, unsigned int numberOfRuns) {
//Variables
	int cnt = 0, printcnt = 0, status;
	short features;
	char calibrateDmp = 0;
	bool endless = BOOL_FALSE;

	if (initialCalibration) {
		features = FEATURES_CAL;
	} else {
		features = FEATURES_RAW;
	}

	//Set number of runs, if endless print
	if (numberOfRuns == 0) {
		numberOfRuns = IMUVIEWER_FREQ;
		endless = BOOL_TRUE;
	}

	//Init
	myprintf(".........Configure MPU and DMP...........\n\r");
	status = configureDMP(features);
	if (status != XST_SUCCESS) {
		return status;
	}

//Calibrate if required
	if (initialCalibration == BOOL_TRUE) {
		myprintf(".........Calibration...........\n\r");
		status = calibrateGyrAcc(CAL_SAMPLES);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

	//Set char for DMP Calibration
	if (dmpCalibration == BOOL_TRUE) {
		calibrateDmp = 1;
	} else {
		calibrateDmp = 0;
	}

	//En- or disable dynamic gyro calibration
	status = dmpGyroCalibration(calibrateDmp);
	if (status != XST_SUCCESS) {
		return status;
	}

//Enable Interrupts
	status = setupDMPInt();
	if (status != XST_SUCCESS) {
		return status;
	}

//Adjust Number of Runs
	numberOfRuns *= (FIFO_RATE / IMUVIEWER_FREQ);

//Get Data with DMP
	myprintf(".........With DMP...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Reset Status
		status = XST_SUCCESS;

		//Wait for Interrupt Interrupt
		if (dmpDataAvailable() == BOOL_TRUE) {
			//Update Data
			status = updateData();

			//Check whether data should be printed
			if (status == XST_SUCCESS) {
				//Increase print counter
				printcnt++;

				//Dont print every data set
				if ((printcnt % (FIFO_RATE / IMUVIEWER_FREQ)) == 0) {
					//Reset printcnt
					printcnt = 0;

					//Print
					printDataWithDMP(sensors, separator);

					//Print new line
					printf("\n\r");

					//For endless prints
					if (endless == BOOL_TRUE) {
						cnt = 0;
					}
				}
			} else if (status == XST_DEVICE_BUSY) {
				return status;
			} else {
				cnt--;
				printcnt--;
			}
		} else {
			cnt--;
		}
	}

//Return
	return XST_SUCCESS;
}

/*
 * Print Data without DMP
 * In: sensors, number of runs (if 0 --> endless loop)
 */
int printDataWithoutDMP(short sensors, char* separator,
		unsigned int numberOfRuns) {
//Variables
	int cnt, status;

//Get Data without DMP
	myprintf(".........Without DMP...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Print
		status = printDataNoDMP(sensors, separator);

		//LED Run if Successful
		if (status == XST_SUCCESS) {
			toggleLed(IMU_MASK);

			//Decrease count if endless print
			if (numberOfRuns < 1) {
				cnt--;
			}
			//Decrease count if not successful
		} else if (status == XST_DEVICE_BUSY) {
			return status;
		} else {
			cnt--;
		}

//		//Decrease count if not successful
//		if (status != XST_SUCCESS || numberOfRuns < 1) {
//			cnt--;
//		}

//Wait
		usleep(1000000 / IMUVIEWER_FREQ); //100ms
	}
	return XST_SUCCESS;
}
