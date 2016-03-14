# Parameters
outfile = "data_raw.pdf";

# Read in data
data = load ('data.txt');
gyr = data(:,1:3);
acc = data(:,4:6);
mag = data(:,7:9);
tem = data(:,10);
pwm = data(:,11:16);
uwb = data(:,17);

# Plot Gyr
plot(gyr(:,1), "r");
hold on;
plot(gyr(:,2), "g");
hold on;
plot(gyr(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Gyroscope');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('x', 'y', 'z');

# Print
print(outfile);
hold off;

# Plot Acc
plot(acc(:,1), "r");
hold on;
plot(acc(:,2), "g");
hold on;
plot(acc(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Accelerometer');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('x', 'y', 'z');

# Print
print("-append", outfile);
hold off;

# Plot Mag
plot(mag(:,1), "r");
hold on;
plot(mag(:,2), "g");
hold on;
plot(mag(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Magnetometer');
xlabel('Sample Number');
ylabel('Magnetic Field Strength (in Hardware Units)');
legend('x', 'y', 'z');

# Print
print("-append", outfile);
hold off;

# Plot Temp
plot(tem, "c");
hold on;

# Set up Plot
grid on;
title('Temperature');
xlabel('Sample Number');
ylabel('Temperature (in Hardware Units)');
legend('Temperature');

# Print
print("-append", outfile);
hold off;

# Plot UWB Distance
plot(uwb, "x");
hold on;

# Set up Plot
grid on;
title('Distance to UWB Receiver');
xlabel('Sample Number');
ylabel('Distance (cm)');
legend('-x');

# Print
print("-append", outfile);
hold off;