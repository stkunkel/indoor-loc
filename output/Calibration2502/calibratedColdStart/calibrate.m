# Parameters
gx_sens = 1;
gx_zlo = -56;
gy_sens = 1.042221751; 
gy_zlo = 17;
gz_sens = 0.985152029;
gz_zlo = -9;
ax_sens = 1.000854492;
ax_zlo = 180;
ay_sens = 1.000671387;
ay_zlo = -226;
az_sens = 0.986572266;
az_zlo = -35;

# Read in data
data = load ('data.txt');
gx = data(:,1);
gy = data(:,2);
gz = data(:,3);
ax = data(:,4);
ay = data(:,5);
az = data(:,6);

# Calibrate
gx_cal = 1/gx_sens * (gx - gx_zlo);
gy_cal = 1/gy_sens * (gy - gy_zlo);
gz_cal = 1/gz_sens * (gz - gz_zlo);
ax_cal = 1/ax_sens * (ax - ax_zlo);
ay_cal = 1/ay_sens * (ay - ay_zlo);
az_cal = 1/az_sens * (az - az_zlo);

# Export Calibrated Data
cal = [gx_cal gy_cal gz_cal ax_cal ay_cal az_cal];
save('calibrated.mat', 'cal');

# Gyroscope
# Plot gx
plot(gx, "r");
hold on;
plot(gx_cal, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (x)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('raw', 'calibrated');

# Print
print("gyroscope_cal.pdf");
hold off;

#Plot gy
plot(gy, "g");
hold on;
plot(gy_cal, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (y)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('raw', 'calibrated');

# Print
print("-append", "gyroscope_cal.pdf");
hold off;

# Plot gz
plot(gz, "b");
hold on;
plot(gz_cal, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (z)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('raw', 'calibrated');

# Print
print("-append", "gyroscope_cal.pdf");
hold off;

# Accelerometer
# Plot ax
plot(ax, "r");
hold on;
plot(ax_cal, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (in Hardware Units)');
legend('raw', 'calibrated');

# Print
print("accelerometer_cal.pdf");
hold off;

#Plot ay
plot(ay, "g");
hold on;
plot(ay_cal, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (y)');
xlabel('Sample Number');
ylabel('Acceleration (in Hardware Units)');
legend('raw', 'calibrated');

# Print
print("-append", "accelerometer_cal.pdf");
hold off;

# Plot gz
plot(az, "b");
hold on;
plot(az_cal, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (z)');
xlabel('Sample Number');
ylabel('Acceleration (in Hardware Units)');
legend('raw', 'calibrated');

# Print
print("-append", "accelerometer_cal.pdf");
hold off;