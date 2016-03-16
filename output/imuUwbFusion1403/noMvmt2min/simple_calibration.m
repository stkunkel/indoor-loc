# Parameters
name = "simple_calibration";
f_norm_filename = "fnorm.mat";
n = 100; %number of samples for calibration
gyr_sens = 32.8;
acc_sens = 8192;

# Determine Export and Outfile Strings
outmat = strcat(name, ".mat");
outfile = strcat(name, ".pdf");

# Offset Removal Function
function out = removeOffset(data, n)
  
  # Get first n values
  part = data(1:n,:);
  
  # Compute Offset
  offset = mean(part);
  
  # Substract Offset
  out = data - offset;
  
  # Print Offset
  printf("Offset: %f\r\n", offset);

endfunction

# Read in data
data = load ('data.txt');
gx = data(:,1);
gy = data(:,2);
gz = data(:,3);
ax = data(:,4);
ay = data(:,5);
az = data(:,6);

# Get Offsets
better_gx = removeOffset(gx, n);
better_gy = removeOffset(gy, n);
better_gz = removeOffset(gz, n);
better_ax = removeOffset(ax, n);
better_ay = removeOffset(ay, n);
better_az = removeOffset((az-8192), n) + 8192;

# Export Calibrated Data
cal = [better_gx better_gy better_gx better_ax better_ay better_az];
save(outmat, 'cal');

# Export normal force
f_norm = [mean(better_ax(1:n)); mean(better_ay(1:n)); mean(better_az(1:n))];
save(f_norm_filename, 'f_norm');

# Gyroscope
# Plot gx
plot(gx, "r");
hold on;
plot(better_gx, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (x)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('raw', 'Offset removed');

# Print
print(outfile);
hold off;

#Plot gy
plot(gy, "g");
hold on;
plot(better_gy, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (y)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('raw', 'Offset removed');

# Print
print("-append", outfile);
hold off;

# Plot gz
plot(gz, "b");
hold on;
plot(better_gz, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope (z)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('raw', 'Offset removed');

# Print
print("-append", outfile);
hold off;

# Accelerometer
# Plot ax
plot(ax, "r");
hold on;
plot(better_ax, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (in Hardware Units)');
legend('raw', 'Offset removed');

# Print
print("-append", outfile);
hold off;

#Plot ay
plot(ay, "g");
hold on;
plot(better_ay, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (y)');
xlabel('Sample Number');
ylabel('Acceleration (in Hardware Units)');
legend('raw', 'Offset removed');

# Print
print("-append", outfile);
hold off;

# Plot gz
plot(az, "b");
hold on;
plot(better_az, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer (z)');
xlabel('Sample Number');
ylabel('Acceleration (in Hardware Units)');
legend('raw', 'Offset removed');

# Print
print("-append", outfile);
hold off;