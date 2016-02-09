# Read in data
data = load ('data.txt');
x = data(:,7);
y = data(:,8);
z = data(:,9);

# Plot all
plot(x, "r");
hold on;
plot(y, "g");
hold on;
plot(z, "b");
hold on;

# Set up Plot
grid on;
title('Magnetometer');
xlabel('Sample Number');
ylabel('Magnetic Field Strength (in Hardware Units)');
legend('x', 'y', 'z');

# Print
print("magnetometer.pdf");