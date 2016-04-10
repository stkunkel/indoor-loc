% Parameters
gab = 0; %0 = fused data, 1=gyroscope only, 2=accelerometer only
rawname = 'quatPos';
rawname_ga = 'q'; % .mat
middlepart = '_simple_cal_fir_hl';
ending = '.mat';
outfile = 'quaternions_compFil';
wndw = 20; % Window for simple calibration

% Load Quaternion pkg
pkg load quaternion;

% Generate File Name Template

if (gab == 0)
	name = strcat(rawname, middlepart, '_compFilter');
	infile = strcat(name, ending);
	load(infile, 'out');
elseif (gab == 1)
	name = strcat(rawname_ga, '_gyr', middlepart, 'qg');
	infile = strcat(name, ending);
	load(infile, 'q_gyr');
	out = q_gyr;
elseif (gab == 2)
	name = strcat(rawname_ga, '_acc', middlepart, 'qa');
	infile = strcat(name, ending);
	load(infile, 'q_acc');
	out = q_acc;
endif;

% Generate Out File Name
outfile = strcat(name, '.eps');

% Load quaternions
ow = out(1:9750,1);
ox = out(1:9750,2);
oy = out(1:9750,3);
oz = out(1:9750,4);

% Print quaternion at 90dgr
printf("Quaternion at 90dgr: %d, %d, %d, %d\r\n", ow(6000), ox(6000), oy(6000), oz(6000));

% Print final quaternion
printf("Final quaternion: %d, %d, %d, %d\r\n", ow(9750), ox(9750), oy(9750), oz(9750));

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

