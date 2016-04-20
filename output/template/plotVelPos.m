% Parameters
filt = 5; %0 = "raw", 1 = "static cal", 2 = "simple cal + mvavg", 3 = "simple cal + fir", 4 = "simple cal + kalman", 5 = "simple cal + no filter", 6 = FIR filter for Comp Filter
imufusion =1; % 0 = no data fusion, 1 = Comp filter,2 = Kalman filter
outfile = 'avs';
wndw = 20; % Window for simple calibration
cof = 0.01;

% Generate File Name Template
rawname = 'avs'; % .mat
if (filt == 0)
	name = rawname;
elseif (filt == 2)
	name = strcat(rawname, '_mvavg');
elseif (filt == 3)
	name = strcat(rawname, '_fir');
elseif (filt == 4)
	name = strcat(rawname, '_kalman');
elseif (filt == 5)
	name = strcat(rawname, '_simple_cal');
elseif (filt == 6)
	name = strcat(rawname, '_simple_cal_fir_hl');
	imufusion = 1;
endif;

% Fused Name
if (imufusion == 0)
	fusedname = strcat(name, '_noFusion');
elseif (imufusion == 1)
	fusedname = strcat(name, '_compFilter');
elseif (imufusion == 2)
	fusedname = strcat(name, '_kalman');
endif;

% Load Data
infile = strcat(fusedname, '.mat');
load(infile, 'avs');

% Get Data
ax = avs(:,1);
ay = avs(:,2);
az = avs(:,3);
vx = avs(:,4);
vy = avs(:,5);
vz = avs(:,6);
sx = avs(:,7);
sy = avs(:,8);
sz = avs(:,9);


% Load Signal Package
pkg load signal

% Plot Acceleration
plot(ax, "r");
hold on;

% Set up Plot
grid on;
title('Acceleration');
xlabel('Sample Number');
ylabel('Acceleration (m/s^2)');
legend('x');
hold off;

% Print Velocity
print('-color', strcat(fusedname, '_acc_x', '.eps'));

% Plot Acceleration
plot(ay, "g");
hold on;

% Set up Plot
grid on;
title('Acceleration');
xlabel('Sample Number');
ylabel('Acceleration (m/s^2)');
legend('y');
hold off;

% Print Velocity
print('-color', strcat(fusedname, '_acc_y', '.eps'));

% Plot Acceleration
plot(az, "b");
hold on;

% Set up Plot
grid on;
title('Acceleration');
xlabel('Sample Number');
ylabel('Acceleration (m/s^2)');
legend('z');
hold off;

% Print Velocity
print('-color', strcat(fusedname, '_acc_z', '.eps'));

% Plot Velocity
plot(vx, "r");
hold on;
plot(vy, "g");
hold on;
plot(vz, "b");
hold on;

% Set up Plot
grid on;
title('Velocity');
xlabel('Sample Number');
ylabel('Velocity (m/s)');
legend('x', 'y', 'z');
hold off;

% Print Velocity
print('-color', strcat(fusedname, '_vel', '.eps'));

% Plot Position
plot(sx, "r");
hold on;
plot(sy, "g");
hold on;
plot(sz, "b");
hold on;

% Set up Plot
grid on;
title('Position');
xlabel('Sample Number');
ylabel('Position (m)');
legend('x', 'y', 'z');
hold off;

% Print Position
print('-color', strcat(fusedname, '_pos', '.eps'));

% Create Filter for Velocity
coeff = fir1(wndw, cof, 'high');
filtered_vx = filter(coeff, 1, vx);
filtered_vy = filter(coeff, 1, vy);
filtered_vz = filter(coeff, 1, vz);

% Plot filtered velocity
% Plot Velocity
plot(filtered_vx, "r");
hold on;
plot(filtered_vy, "g");
hold on;
plot(filtered_vz, "b");
hold on;

% Set up Plot
grid on;
title('Filtered Velocity (High-pass FIR Filter)');
xlabel('Sample Number');
ylabel('Velocity (m/s)');
legend('x', 'y', 'z');
hold off;

% Print Velocity
print('-color', strcat(fusedname, '_filtered_vel', num2str(cof), '.eps'));