% Parameters
wndw = 20*500/20; %20 * freq_IMU / freq_UWB
val = 1/50;
name = 'avs_simple_cal_compFilter';
avs_infile = strcat(name, '.mat');
outfile = strcat(name, '_hpf');

% Load Signal Package
pkg load signal

% Load Data from IMU
load(avs_infile, 'avs');

%FIR Filter Coefficients
coeff = fir1(wndw, val, 'high');

%Apply Filter
data = filter(coeff, 1, avs);

% Plot
plot(avs(:,7), 'r');
hold on;
plot(data(:,7), 'c');
hold on;

% Set up Plot
grid on;
title('Filtered Position (IMU)');
xlabel('Sample Number');
ylabel('Position (m)');
legend('raw', 'filtered');

% Print
print(strcat(outfile, '.pdf'));
print(strcat(outfile, '.eps'));

% Export
avs = data;
save('avs', strcat(outfile, '.mat'));
