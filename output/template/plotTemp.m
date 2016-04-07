% Read in data
data = load ('data.txt');
t = data(:,10);

% Plot
plot(t, "c");
hold on;

% Set up Plot
grid on;
title('Temperature');
xlabel('Sample Number');
ylabel('Temperature (in Hardware Units)');
legend('Temperature');

% Print
print("temperature.pdf");