% Read in data
data = load ('data.txt');
x = data(:,1);
y = data(:,2);
z = data(:,3);

% Plot all
plot(x, "r");
hold on;
plot(y, "g");
hold on;
plot(z, "b");
hold on;

% Set up Plot
grid on;
title('Gyroscope');
xlabel('Sample Number');
ylabel('Angular Velocity (in Hardware Units)');
legend('x', 'y', 'z');

% Print
print("gyroscope.pdf");


% Clear graph
hold off;

% Fourier Transform
fx = fft(x);
fy = fft(y);
fz = fft(z);

% Plot fx
plot(abs(fx), "r");
hold on;

% Set up plot
title('Gyroscope FFT');
xlabel('');
ylabel('|F|');
legend('x');

% Print fx
print("gyroscope_fft.pdf");
hold off;

% Plot fy
plot(abs(fy), "g");
hold on;

% Set up plot
title('Gyroscope FFT');
xlabel('');
ylabel('|F|');
legend('y');

% Print fy
print("-append", "gyroscope_fft.pdf");
hold off;

% Plot fz
plot(abs(fz), "b");
hold on;

% Set up plot
title('Gyroscope FFT');
xlabel('');
ylabel('|F|');
legend('z');

% Print fz
print("-append", "gyroscope_fft.pdf");
hold off;