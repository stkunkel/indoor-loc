Gyro Sens: 500°/s
Accel Sens: 4G
no calibration (i.e. raw data)
no DMP
2min sampling time per set
Servo Motors turned off after reset

No Movements

FIR Filter (20, 0.1)

First 20 filtered values were removed.

Sample Sets:
1.) Wrist moves 0° - 90° - 0° - ... --> Gyro Sensitivity (y-axis)
2.) Base moves 0° - 90° - 0° - ... --> Gyro Sensitivity (z-axis)
3.) x-axis faces down --> Accel ZLO/Sens (-x)
4.) x-axis faces up --> Accel ZLO/Sens (+x)
5.) y-axis faces down --> Accel ZLO/Sens (-y)
6.) y-axis faces up --> Accel ZLO/Sens (+y)
7.) All angles 0°, no motion --> Gyro Zero-level offset, Accel Sens and ZLO: z-axis faces up (z+)

#################################
Raw Results:
#################################
Gyro 
#################################
ZLO (0°):
x: Avg(x): -56
y: Avg(y): 17
z: Avg(z): -9

Sensitivity (90°):
x: N/A
y: Max(-y): 10116, Max(+y): 12587
z: Max(-z): 10183, Max(+z): 10206

#################################
Accel
#################################
ZLO (0G):
x: Avg(x): 137	Avg(x): 71	Avg(x): 331 			--> ZLO = Avg = 180
y: Avg(y): -207	Avg(y): -209	Avg(y): -263 			--> ZLO = Avg = 226
z: Avg(z): 105	Avg(z): 194	Avg(z): 143	Avg(z): -585 	--> ZLO = Avg = -35

Sensitivity (1G):
x: Avg(+x): 8118, Avg(-x): 8280 --> Avg = 8199 		--> divided by 8192 (für Accel FSR of 4G) --> Sens = 1.000854492
y: Avg(+y): 8166, Avg(-y): 8229 --> Avg = 8197.5 	--> divided by 8192 (für Accel FSR of 4G) --> Sens = 1.000671387
z: Avg(+z): 8082 					--> divided by 8192 (für Accel FSR of 4G) --> Sens = 0.986572266

