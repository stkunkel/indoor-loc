# Parameters
q = 0.125;
r = 4.0;

# Variables
p = 30000.0;
x = 0.0;
k = 1.0;

# Read in data
data = load ('data.txt');
ax = data(:,4);
ay = data(:,5);
az = data(:,6);

# Initialize
x = ax(1);
filtered_x = x;

# Go through samples
for i=2:size(ax,1)

  # Prediction Update
  p = p + q;

  # Measurement Update
  k = p / (p + r);
  x = x + k*(ax(i) -x);
  p = (1-k)*p;
  
  # Append filtered value to result vector
  filtered_x = horzcat(filtered_x, x);
end

# Plot x
plot(ax, "r");
hold on;
plot(filtered_x, "b");
hold off;

# Setup Plot for x
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (in Hardware Units)');
legend('raw', 'Kalman filter');
xlim([2 1000]);

# Print
#print( "accelerometer_Kalman1D.pdf");