#Read in data
data = load ('data.txt');
x = data(:,4);
y = data(:,5);
z = data(:,6);

#Plot all
plot(x, "r");
hold on;
plot(y, "g");
hold on;
plot(z, "b");
hold on;

#Set up Plot
grid on;
title('Accelerometer');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('x', 'y', 'z');

#Print
print("accelerometer.pdf");

# Clear graph
hold off;

# Fourier Transform
fx = fft(x);
fy = fft(y);
fz = fft(z);

# Plot fx
plot(abs(fx), "r");
hold on;

# Set up plot
title('Accelerometer FFT');
xlabel('');
ylabel('|F|');
legend('x');

# Print fx
print("accelerometer_fft.pdf");
hold off;

# Plot fy
plot(abs(fy), "g");
hold on;

# Set up plot
title('Accelerometer FFT');
xlabel('');
ylabel('|F|');
legend('y');

# Print fy
print("-append", "accelerometer_fft.pdf");
hold off;

# Plot fz
plot(abs(fz), "b");
hold on;

# Set up plot
title('Accelerometer FFT');
xlabel('');
ylabel('|F|');
legend('z');

# Print fz
print("-append", "accelerometer_fft.pdf");
hold off;