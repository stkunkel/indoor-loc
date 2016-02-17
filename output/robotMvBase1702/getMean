# Read in data
data = load ('filtered.txt');
x = data(:,4);
y = data(:,5);
z = data(:,6);

# Create Sub matrix of first 30 values
x_small = x(1:30,:);
y_small = y(1:30,:);
z_small = z(1:30,:);

# Print mean of sub matrices
printf("Mean x: %f, y: %f, z: %f\r\n", mean(x_small), mean(y_small), mean(z_small));