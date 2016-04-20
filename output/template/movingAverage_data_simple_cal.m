% Parameters
gyr_sens = 32.8;
acc_sens = 8192;
wndw = 20;
outfile = "mvAvg_simple_cal.pdf";

% Load Calibrated Data
load('simple_calibration.mat', 'cal');
gx = cal(:,1);
gy = cal(:,2);
gz = cal(:,3);
ax = cal(:,4);
ay = cal(:,5);
az = cal(:,6);


%Compute Moving Average
gx_mvavg = filter(ones(1, wndw)/wndw, 1, gx);
gy_mvavg = filter(ones(1, wndw)/wndw, 1, gy);
gz_mvavg = filter(ones(1, wndw)/wndw, 1, gz);
ax_mvavg = filter(ones(1, wndw)/wndw, 1, ax);
ay_mvavg = filter(ones(1, wndw)/wndw, 1, ay);
az_mvavg = filter(ones(1, wndw)/wndw, 1, az);

% Discard first and last values
l = length(gx);
filtered_gx = gx_mvavg(wndw+1:l-wndw,:);
filtered_gy = gy_mvavg(wndw+1:l-wndw,:);
filtered_gz = gz_mvavg(wndw+1:l-wndw,:);
filtered_ax = ax_mvavg(wndw+1:l-wndw,:);
filtered_ay = ay_mvavg(wndw+1:l-wndw,:);
filtered_az = az_mvavg(wndw+1:l-wndw,:);

% Export Filtered
mvavg = [filtered_gx filtered_gy filtered_gz filtered_ax filtered_ay filtered_az];
save('mvavg_simple_cal.mat', 'mvavg');

% Print statistics
printf("Gyroscope:\r\n");
printf("x: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_gx), median(filtered_gx), min(filtered_gx), max(filtered_gx), range(filtered_gx), std(filtered_gx));
printf("y: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_gy), median(filtered_gy), min(filtered_gy), max(filtered_gy), range(filtered_gy), std(filtered_gy));
printf("z: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_gz), median(filtered_gz), min(filtered_gz), max(filtered_gz), range(filtered_gz), std(filtered_gz));
printf("Accelerometer:\r\n");
printf("x: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_ax), median(filtered_ax), min(filtered_ax), max(filtered_ax), range(filtered_ax), std(filtered_ax));
printf("y: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_ay), median(filtered_ay), min(filtered_ay), max(filtered_ay), range(filtered_ay), std(filtered_ay));
printf("z: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_az), median(filtered_az), min(filtered_az), max(filtered_az), range(filtered_az), std(filtered_az));

% Gyroscope
% Plot gx
plot(gx, "r");
hold on;
plot(filtered_gx, "c");
hold on;

% Set up Plot
grid on;
title('Gyroscope (x)');
xlabel('Sample Number');
ylabel('Angular Velocity (Hardware Units)');
legend('Calibrated', 'Moving Average');
hold off;

% Print Plot
print(outfile);

% Plot gy
plot(gy, "g");
hold on;
plot(filtered_gy, "c");
hold on;

% Set up Plot
grid on;
title('Gyroscope (y)');
xlabel('Sample Number');
ylabel('Angular Velocity (Hardware Units)');
legend('Calibrated', 'Moving Average');
hold off;

% Print Plot
print("-append", outfile);

% Plot gz
plot(gz, "b");
hold on;
plot(filtered_gz, "c");
hold on;

% Set up Plot
grid on;
title('Gyroscope (z)');
xlabel('Sample Number');
ylabel('Angular Velocity (Hardware Units)');
legend('Calibrated', 'Moving Average');
hold off;

% Print Plot
print("-append", outfile);

% Accelerometer
% Plot ax
plot(ax, "r");
hold on;
plot(filtered_ax, "c");
hold on;

% Set up Plot
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Moving Average');
hold off;

% Print Plot
print("-append", outfile);

% Plot some Accelerometer Data
filtered_ax2 = [zeros(wndw/2,1); filtered_ax; zeros(wndw/2,1)];
plot(ax(wndw+1:wndw+100), "r");
hold on;
plot(filtered_ax2(wndw+1:wndw+100), "c");
hold on;

% Set up Plot
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Moving Average');
hold off;

% Print
print('-color', 'accel_mvavg.eps');
hold off;

% Plot ay
plot(ay, "g");
hold on;
plot(filtered_ay, "c");
hold on;

% Set up Plot
grid on;
title('Accelerometer (y)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Moving Average');
hold off;

% Print Plot
print("-append", outfile);

% Plot az
plot(az, "b");
hold on;
plot(filtered_az, "c");
hold on;

% Set up Plot
grid on;
title('Accelerometer (z)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Moving Average');
hold off;

% Print Plot
print("-append", outfile);