Create Filtered Data and Compute Position Locally in IMU Viewer:

1. Set up Kermin or similar to receive raw sensor data:
	$ kermit
	kermit> log session <log file name>
	conn

2. In XSDK, specify calibration and sampling time of collectRegisterData in run.c. If calibration is required, uncomment INITIAL_CALIBRATION define in program_parameters.h. Also comment USE_DMP and DEBUG defines. In run.c, make sure only collectRegisterData method is executed in main function. Run application.

3. Format kermit log file: replace ";" by new line: 
	$ cat <log file> | tr ';' '\n' > data.txt

4. In data.txt, remove any incomplete lines. If calibration has been done, the first line contains the average raw accelerations during calibration, which will be required later on.

5. Run Octave scripts, especially createFilteredData and, if there was no initial calibration, getMeanOfFirstTenValues to get values used as calibration values later on.
	$ octave createFilteredData

6. Copy filtered.txt into imu_viewer_osg folder.

7. Modify defines localProcessing/program_parameters.h:
	#define NORM_MEAS_X/Y/Z (<raw x/y/z calibration value>/ACCEL_CONV_VAL)

8. Build imuviewer executable in imu_viewer_osg folder using the existing makefile (may need adjustments):
	$ make

9. Run ./imuviewer

10. Copy out.txt into fir0.1/ folder in the corresponding folder in the output folder
11. Run octave script to create Plot:
	$ octave plotOut
