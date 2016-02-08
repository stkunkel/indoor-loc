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
#include "print_utils.h"
#include "imu/mpu.h"
#include "imu/mpu_int.h"
#include "robot/pwmsw.h"
#include "zedboard/gpio_utils.h"
#include "zedboard/timer_int.h"
#include "bool.h"
#include "program_parameters.h"

/*
 * Defines
 */
#define DATA_NO_DMP_RUNS 	0
#define DATA_WITH_DMP_RUNS 	0
#define DISPLAY_RUNS		0
#define QUAT_DRIFT_MIN		1
#define SEPARATOR			" "

/*
 * Function Prototypes
 */
int printQuaternionDriftAfterXMin(unsigned int time_min);
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
	//Variables
	int status = XST_SUCCESS;

	//Init Platform
	init_platform();

	//Init GPIO (for LEDs)
	initGpio();

	//Print Output Separator
	myprintf(".........Program Start...........\n\r");

	//Print Data without DMP
//	status = printDataWithoutDMP(SENSORS_ALL, SEPARATOR, DATA_NO_DMP_RUNS);

//Print Data with DMP without initial or DMP gyro calibration
//	status = printDataUsingDMP(SENSORS_ALL, 0, 0, SEPARATOR, DATA_WITH_DMP_RUNS);

//Print Data with DMP with initial calibration but no DMP gyro calibration
//	status = printDataUsingDMP(SENSORS_ALL, 1, 0, SEPARATOR, DATA_WITH_DMP_RUNS);

//Print Data with DMP with and DMP gyro calibration
//	status = printDataUsingDMP(SENSORS_ALL, 1, 1, SEPARATOR, DATA_WITH_DMP_RUNS);

//Print Quaternions and Position to Serial Port
//	status = printForImuViewer(PRINT_ALL, SEPARATOR, DISPLAY_RUNS);

//	Generic Print to Serial Port
//	status = printGeneric(PRINT_ACCEL, SEPARATOR, DISPLAY_RUNS);

//Quaternion Drift
//status = printQuaternionDriftAfterXMin(QUAT_DRIFT_MIN);

//PWM Test
//status = pwmTest();

//	//Timer Test
//	status = timerTest(FIFO_RATE);

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

//Test LED
//	testToggleLed();
//	testLedRun();

//Collect Data
	collectRegisterData(600, 60); //10min sampling, 1min calibration

//Stay in here
	while (1) {
		;
	}

	//Finish
	cleanup_platform();
	return 0;
}

/*
 * Prints Quaternion Drift
 * In: time in min
 * Returns 0 when done
 */
int printQuaternionDriftAfterXMin(unsigned int time_min) {
	//Print
	myprintf(".........Drift...........\n\r");
	printQuatDrift(time_min);

	//Return
	return XST_SUCCESS;
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
					status = printforDisplay(&printMask, separator);

					//Print new line
					if (status == XST_SUCCESS) {
						//Increase print counter
						printcnt++;

						//Print new linw
						printf("\n\r");
					} else {
						printcnt--;
					}

					//For endless prints
					if (endless == BOOL_TRUE) {
						printcnt = 0;
					}

				}
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
					status = printforDisplay(&printMask, separator);

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
			ledRun();

			//Decrease count if endless print
			if (numberOfRuns < 1) {
				cnt--;
			}
			//Decrease count if not successful
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
