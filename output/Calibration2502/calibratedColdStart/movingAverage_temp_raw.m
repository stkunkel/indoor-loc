# Parameters
wndw = 20;
outfile = "mvAvg_temp_raw.pdf";

# Load Calibrated Data
data = load ('data.txt');
temp_raw = data(:,10);


# Compute Moving Average
temp_mvavg = filter(ones(1, wndw)/wndw, 1, temp_raw);


# Discard first and last values
l = length(temp_raw);
filtered_temp = temp_mvavg(wndw+1:l-wndw,:);


# Export Filtered
save('temp_mvavg.mat', 'temp_mvavg');

# Print statistics
printf("Temperature:\r\n");
printf("Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(filtered_temp), median(filtered_temp), min(filtered_temp), max(filtered_temp), range(filtered_temp), std(filtered_temp));

# Gyroscope
# Plot gx
plot(temp_raw, "c");
hold on;
plot(filtered_temp, "b");
hold on;

# Set up Plot
grid on;
title('Temperature');
xlabel('Sample Number');
ylabel('Temperature (Hardware Units)');
legend('Raw', 'Moving Average');
hold off;

# Print Plot
print(outfile);