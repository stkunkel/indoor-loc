% Parameters
filt = 2; %0 = "raw", 1 = "static cal", 2 = "simple cal + mvavg", 3 = "simple cal + fir", 4 = "simple cal + kalman", 5 = "simple cal + no filter"
outfile = 'quaternions';
wndw = 20; % Window for simple calibration

% Load Quaternion pkg
pkg load quaternion;

% Generate File Name Template
rawname = 'quat'; % .mat
if (filt == 0)
	name = rawname;
	infile = strcat(name, '.mat');
	load(infile, 'out');
	q = out;
elseif (filt == 1)
	name = strcat(rawname, '_cal');
	infile = strcat(name, '.mat');
	load(infile, 'quat_cal');
	q = quat_cal;
elseif (filt == 2)
	name = strcat(rawname, '_mvavg_noFusion');
	infile = strcat(name, '.mat');
	load(infile, 'out');
	q = out;
elseif (filt == 3)
	name = strcat(rawname, '_fir_noFusion');
	infile = strcat(name, '.mat');
	load(infile, 'out');
	q = out;
elseif (filt == 4)
	name = strcat(rawname, '_kalman_noFusion');
	infile = strcat(name, '.mat');
	load(infile, 'out');
	q = out;
elseif (filt == 5)
	name = strcat(rawname, '_simple_cal');
	infile = strcat(name, '.mat');
	load(infile, 'out');
	q = out;
		
endif;

% Generate Out File Name
outfile = strcat(name, '.eps');


% Load quaternions
ow = q(:,1);
ox = q(:,2);
oy = q(:,3);
oz = q(:,4);

% Plot Quaternion
plot(ow, "c");
hold on;
plot(ox, "r");
hold on;
plot(oy, "g");
hold on;
plot(oz, "b");
hold on;

% Set up Plot
grid on;
title('Quaternions');
xlabel('Sample Number');
ylabel('Value');
legend('w', 'x', 'y', 'z');

% Print
print('-color', outfile);

