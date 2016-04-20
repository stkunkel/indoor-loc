# Parameters
outfile = "kalmanFilter_cal.pdf";
q = 0.125; 	#the smaller the less noise
r = 4.0;	#the higher the more stable, the lower the more filter relies on sensor data
wndw = 20;

#Kalman Function
function filtered = kalman1d(q, r, p, x ,k, data)

  # Initialize
  x = data(1);
  filtered = x;

  # Go through samples
  for i=2:size(data,1)

    # Prediction Update
    p = p + q;

    # Measurement Update
    k = p / (p + r);
    x = x + k*(data(i) -x);
    p = (1-k)*p;
    
    # Append filtered value to result vector
    filtered = horzcat(filtered, x);
  end

endfunction

# Variables
p = 30000.0;
x = 0.0;
k = 1.0;

# Load Calibrated Data
load ('simple_calibration.mat', 'cal');
gx = cal(:,1);
gy = cal(:,2);
gz = cal(:,3);
ax = cal(:,4);
ay = cal(:,5);
az = cal(:,6);

# Apply Filter
filtered_gx = kalman1d(q, r, p, x, k, gx);
filtered_gy = kalman1d(q, r, p, x, k, gy);
filtered_gz = kalman1d(q, r, p, x, k, gz);
filtered_ax = kalman1d(q, r, p, x, k, ax);
filtered_ay = kalman1d(q, r, p, x, k, ay);
filtered_az = kalman1d(q, r, p, x, k, az);

# Export Filtered
kalmanfil = [rot90(filtered_gx, -1) rot90(filtered_gy, -1) rot90(filtered_gz, -1) rot90(filtered_ax, -1) rot90(filtered_ay, -1) rot90(filtered_az, -1)];
save('kalman_simple_cal.mat', 'kalmanfil');

# Print statistics
printf("Gyroscope:\r\n");
printf("x: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_gx), median(filtered_gx), min(filtered_gx), max(filtered_gx), range(filtered_gx), std(filtered_gx));
printf("y: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_gy), median(filtered_gy), min(filtered_gy), max(filtered_gy), range(filtered_gy), std(filtered_gy));
printf("z: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_gz), median(filtered_gz), min(filtered_gz), max(filtered_gz), range(filtered_gz), std(filtered_gz));
printf("Accelerometer:\r\n");
printf("x: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_ax), median(filtered_ax), min(filtered_ax), max(filtered_ax), range(filtered_ax), std(filtered_ax));
printf("y: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_ay), median(filtered_ay), min(filtered_ay), max(filtered_ay), range(filtered_ay), std(filtered_ay));
printf("z: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_az), median(filtered_az), min(filtered_az), max(filtered_az), range(filtered_az), std(filtered_az));

# Gyroscope
# Plot gx
plot(gx, "r");
hold on;
plot(filtered_gx, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (x)');
xlabel('Sample Number');
ylabel('Angular Velocity (Hardware Units)');
legend('Calibrated', 'Kalman Filter');
hold off;

# Print Plot
print(outfile);

# Plot gy
plot(gy, "g");
hold on;
plot(filtered_gy, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (y)');
xlabel('Sample Number');
ylabel('Angular Velocity (Hardware Units)');
legend('Calibrated', 'Kalman Filter');
hold off;

# Print Plot
print("-append", outfile);

# Plot gz
plot(gz, "b");
hold on;
plot(filtered_gz, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (z)');
xlabel('Sample Number');
ylabel('Angular Velocity (Hardware Units)');
legend('Calibrated', 'Kalman Filter');
hold off;

# Print Plot
print("-append", outfile);

# Accelerometer
# Plot ax
plot(ax, "r");
hold on;
plot(filtered_ax, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Kalman Filter');
hold off;

# Print Plot
print("-append", outfile);

# Plot some Accelerometer Data
plot(ax(wndw+1:wndw+100), "r");
hold on;
plot(filtered_ax(wndw+1:wndw+100), "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Kalman Filter');
hold off;

# Print
print('-color', 'accel_kalman.eps');
hold off;

# Plot ay
plot(ay, "g");
hold on;
plot(filtered_ay, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (y)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Kalman Filter');
hold off;

# Print Plot
print("-append", outfile);

# Plot az
plot(az, "b");
hold on;
plot(filtered_az, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (z)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Kalman Filter');
hold off;

# Print Plot
print("-append", outfile);