# Parameters
outfile = "allan_variance.pdf";
f = 500; %sampling frequency in Hz
gyr_sens = 32.8;
acc_sens = 8192;

# Variables
sum_gx = 0;
sum_gy = 0;
sum_gz = 0;
sum_ax = 0;
sum_ay = 0;
sum_az = 0;

# Read in data
data = load ('data.txt');
gx_raw = data(:,1);
gy_raw = data(:,2);
gz_raw = data(:,3);
ax_raw = data(:,4);
ay_raw = data(:,5);
az_raw = data(:,6);

# Convert data
gx = gx_raw / gyr_sens;
gy = gy_raw / gyr_sens;
gz = gz_raw / gyr_sens;
ax = ax_raw / acc_sens;
ay = ay_raw / acc_sens;
az = (az_raw - acc_sens) / acc_sens; %remove gravity

# Remove gravity
az = az - grav_z;

# Number of Bins and Bin Size
t = 1/f;
n = length(gx)/t;

# Bin Vector
nv = [1:(n-1)];

# Go through bins
for i=1:(n-1)
	
	# Average of Bins
	a_gx(i) = mean(gx((i*t):(i*t+(t-1))));
	a_gy(i) = mean(gy((i*t):(i*t+(t-1))));
	a_gz(i) = mean(gz((i*t):(i*t+(t-1))));
	a_ax(i) = mean(ax((i*t):(i*t+(t-1))));
	a_ay(i) = mean(ay((i*t):(i*t+(t-1))));
	a_az(i) = mean(az((i*t):(i*t+(t-1))));
	
	# Sums for Allan Variance
	if (i > 1)
		sum_gx += (a_gx(i) - a_gx(i-1))^2;
		sum_gy += (a_gy(i) - a_gy(i-1))^2;
		sum_gz += (a_gz(i) - a_gz(i-1))^2;
		sum_ax += (a_ax(i) - a_ax(i-1))^2;
		sum_ay += (a_ay(i) - a_ay(i-1))^2;
		sum_az += (a_az(i) - a_az(i-1))^2;
	endif;
endfor;

# Allan Variance
avar_gx = sum_gx / (2*(n-1));
avar_gy = sum_gy / (2*(n-1));
avar_gz = sum_gz / (2*(n-1));
avar_ax = sum_ax / (2*(n-1));
avar_ay = sum_ay / (2*(n-1));
avar_az = sum_az / (2*(n-1));

# Allan Deviation
ad_gx = sqrt(avar_gx);
ad_gy = sqrt(avar_gy);
ad_gz = sqrt(avar_gz);
ad_ax = sqrt(avar_ax);
ad_ay = sqrt(avar_ay);
ad_az = sqrt(avar_az);

# Use Absolute Values for Plot
a_gx = abs(a_gx);
a_gy = abs(a_gy);
a_gz = abs(a_gz);
a_ax = abs(a_ax);
a_ay = abs(a_ay);
a_az = abs(a_az);

# Plot Gyro
loglog(nv, a_gx, "r");
hold on;
loglog(nv, a_gy, "g");
hold on;
loglog(nv, a_gz, "b");
hold on;

# Setup Plot
grid on;
title('Gyroscope Allan Deviation');
xlabel('Averaging Time (s)');
ylabel('Allan Deviation (dgr/s)');
legend('|x|', '|y|', '|z|');

# Print
print(outfile);
hold off;

# Plot Accel
loglog(nv, a_ax, "r");
hold on;
loglog(nv, a_ay, "g");
hold on;
loglog(nv, a_az, "b");
hold on;

# Setup Plot
grid on;
title('Accelerometer Allan Deviation');
xlabel('Averaging Time (s)');
ylabel('Allan Deviation (G)');
legend('|x|', '|y|', '|z|');

# Print
print("-append", outfile);
hold off;