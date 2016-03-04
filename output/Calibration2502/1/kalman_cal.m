# Parameters
outfile = "kalmanFilter_cal.pdf";
gyr_sens = 32.8;
acc_sens = 8192;
q = 0.125; 	#the smaller the less noise
r = 10.0;	#the higher the more stable, the lower the more filter relies on sensor data
q_acc = 0.0004; %taken from data sheet
q_gyr = 0.005; %taken from data sheet
values_discard = 40;

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
load ('calibrated.mat', 'cal');
gx = cal(:,1);
gy = cal(:,2);
gz = cal(:,3);
ax = cal(:,4);
ay = cal(:,5);
az = cal(:,6);

# Apply Filter
gx_kalman = kalman1d(q_gyr, r, p, x, k, gx);
gy_kalman = kalman1d(q_gyr, r, p, x, k, gy);
gz_kalman = kalman1d(q_gyr, r, p, x, k, gz);
ax_kalman = kalman1d(q_acc, r, p, x, k, ax);
ay_kalman = kalman1d(q_acc, r, p, x, k, ay);
az_kalman = kalman1d(q_acc, r, p, x, k, az);

# Remove first values
filtered_gx = gx_kalman(values_discard:end);
filtered_gy = gy_kalman(values_discard:end);
filtered_gz = gz_kalman(values_discard:end);
filtered_ax = ax_kalman(values_discard:end);
filtered_ay = ay_kalman(values_discard:end);
filtered_az = az_kalman(values_discard:end);

# Export Filtered
kalmanfil = [rot90(filtered_gx, -1) rot90(filtered_gy, -1) rot90(filtered_gz, -1) rot90(filtered_ax, -1) rot90(filtered_ay, -1) rot90(filtered_az, -1)];
save('kalman.mat', 'kalmanfil');

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
legend('Calibrated', strcat("Kalman Filter (q = ", num2str(q_gyr), ", r = ", num2str(r), ")"));
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
legend('Calibrated', strcat("Kalman Filter (q = ", num2str(q_gyr), ", r = ", num2str(r), ")"));
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
legend('Calibrated', strcat("Kalman Filter (q = ", num2str(q_gyr), ", r = ", num2str(r), ")"));
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
legend('Calibrated', strcat("Kalman Filter (q = ", num2str(q_acc), ", r = ", num2str(r), ")"));
hold off;

# Print Plot
print("-append", outfile);

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
legend('Calibrated', strcat("Kalman Filter (q = ", num2str(q_acc), ", r = ", num2str(r), ")"));
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
legend('Calibrated', strcat("Kalman Filter (q = ", num2str(q_acc), ", r = ", num2str(r), ")"));
hold off;

# Print Plot
print("-append", outfile);