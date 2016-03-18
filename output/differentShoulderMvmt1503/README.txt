Gyro Sens: 1000°/s
Accel Sens: 4G
no calibration (i.e. raw data)
no DMP
2min sampling time per set
Servo Motors still on after reset

IMUs x axis faced backwards (i.e. robots movements were towards neg. x axis).

Normal Force computed as mean of first 100 values

FIR Filter (20, 0.1)
Moving Average Window: 20

First 20 filtered values were removed.

Sample Set: Shoulder moves 90°, waits a bit, goes to 60°, waits a bit, goes to 30°, waits a bit, goes to 0°. Same idea backwards.

