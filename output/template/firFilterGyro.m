# Load Signal Package
pkg load signal

# Read in data
data = load ('data.txt');
x = data(:,1);
y = data(:,2);
z = data(:,3);

# FIR Filter Coefficients
coeff = fir1(20, 0.1, 'low');

# Filter data
#filtered_x = filter2(coeff, x);
#filtered_y = filter2(coeff, y);
#filtered_z = filter2(coeff, z);
filtered_x = filter(coeff, 1, x);
filtered_y = filter(coeff, 1, y);
filtered_z = filter(coeff, 1, z);

# Print Statistics to Console
printf("x (filtered): Mean: %f (%f) \t Min: %d (%f) \t Max: %d (%f) \t Median: %d (%f)\n", mean(x), mean(filtered_x), min(x), min(filtered_x), max(x), max(filtered_x), median(x), median(filtered_x));
printf("y (filtered): Mean: %f (%f) \t Min: %d (%f) \t Max: %d (%f) \t Median: %d (%f)\n", mean(y), mean(filtered_y), min(y), min(filtered_y), max(y), max(filtered_y), median(y), median(filtered_y));
printf("z (filtered): Mean: %f (%f) \t Min: %d (%f) \t Max: %d (%f) \t Median: %d (%f)\n", mean(z), mean(filtered_z), min(z), min(filtered_z), max(z), max(filtered_z), median(z), median(filtered_z));
printf("\r\n");

# Plot x
plot(x, "r");
hold on;
plot(filtered_x, "b");
hold off;

# Setup Plot for x
grid on;
title('Gyroscope (x)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
#ylim([-30, 30]);
legend('unfiltered', 'FIR filter (20, 0.1)');

# Print
print("gyroscope_fir.pdf");

# Plot y
plot(y, "r");
hold on;
plot(filtered_y, "b");
hold off;

# Setup Plot for y
grid on;
title('Gyroscope (y)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
#ylim([-30, 30]);
legend('unfiltered', 'FIR filter (20, 0.1)');

# Print
print("-append", "gyroscope_fir.pdf");

# Plot z
plot(z, "r");
hold on;
plot(filtered_z, "b");
hold off;

# Setup Plot for z
grid on;
title('Gyroscope (z)');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
#ylim([-30, 30]);
legend('unfiltered', 'FIR filter (20, 0.1)');

# Print
print("-append", "gyroscope_fir.pdf");