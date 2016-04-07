% Load Signal Package
pkg load signal

% Parameters
gyr_sens = 32.8;
acc_sens = 8192;
wndw = 20;
lpf = 0.001;
hpf = 0.01;
outfile = strcat("firFilter_cal_h", num2str(hpf), "l", num2str(lpf));

% Load Calibrated Data
load ('simple_calibration.mat', 'cal');
gx = cal(:,1);
gy = cal(:,2);
gz = cal(:,3);
ax = cal(:,4);
ay = cal(:,5);
az = cal(:,6);

% FIR Filter Coefficients
cg = fir1(wndw, hpf, 'high');
ca = fir1(wndw, lpf, 'low');

% Apply Filter
gx_fir = filter(cg, 1, gx);
gy_fir = filter(cg, 1, gy);
gz_fir = filter(cg, 1, gz);
ax_fir = filter(ca, 1, ax);
ay_fir = filter(ca, 1, ay);
az_fir = filter(ca, 1, az);

% Discard first and last values
l = length(gx);
filtered_gx = gx_fir(wndw+1:l-wndw,:);
filtered_gy = gy_fir(wndw+1:l-wndw,:);
filtered_gz = gz_fir(wndw+1:l-wndw,:);
filtered_ax = ax_fir(wndw+1:l-wndw,:);
filtered_ay = ay_fir(wndw+1:l-wndw,:);
filtered_az = az_fir(wndw+1:l-wndw,:);

% Export Filtered
firfil = [filtered_gx filtered_gy filtered_gz filtered_ax filtered_ay filtered_az];
save('fir_hl.mat', 'firfil');

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
legend('Calibrated', 'High-pass FIR Filter');
hold off;

% Print Plot
print(strcat(outfile,".pdf"));

% Plot some gx
filtered_gx2 = [zeros(wndw/2,1); filtered_gx; zeros(wndw/2,1)];
plot(gx(wndw+2000:wndw+5800), "r");
hold on;
plot(filtered_gx2(wndw+2000:wndw+5800), "c");
hold on;

% Set up Plot
grid on;
title('Gyroscope (x)');
xlabel('Sample Number');
ylabel('Angular Velocity (Hardware Units)');
legend('Calibrated', 'High-pass FIR Filter');
hold off;

% Print
print("-color", strcat(outfile,"gx.eps"));

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
legend('Calibrated', 'High-pass FIR Filter');
hold off;

% Print Plot
print("-append", strcat(outfile,".pdf"));

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
legend('Calibrated', 'High-pass FIR Filter');
hold off;

% Print Plot
print("-append", strcat(outfile,".pdf"));

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
legend('Calibrated', 'Low-pass FIR Filter');
hold off;

% Print Plot
print("-append", strcat(outfile,".pdf"));

% Plot some ax
filtered_ax2 = [zeros(wndw/2,1); filtered_ax; zeros(wndw/2,1)];
plot(ax(wndw+2000:wndw+5800), "r");
hold on;
plot(filtered_ax2(wndw+2000:wndw+5800), "c");
hold on;

% Set up Plot
grid on;
title('Accelerometer (x)');
xlabel('Sample Number');
ylabel('Acceleration (Hardware Units)');
legend('Calibrated', 'Low-pass FIR Filter');
hold off;

% Print
print("-color", strcat(outfile,"ax.eps"));

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
legend('Calibrated', 'Low-pass FIR Filter');
hold off;

% Print Plot
print("-append", strcat(outfile,".pdf"));

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
legend('Calibrated', 'Low-pass FIR Filter');
hold off;

% Print Plot
print("-append", strcat(outfile,".pdf"));
