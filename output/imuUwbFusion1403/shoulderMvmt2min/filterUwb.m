# Parameters
wndw = 20;
name = "filteredUwb";

# Generate Filenames
outfile = strcat(name, ".pdf");
outmat = strcat(name, ".mat");

# Load UWB Data
data = load('data.txt');
uwb_raw = data(:,17);

#Compute Moving Average
uwb_mvavg = filter(ones(1, wndw)/wndw, 1, uwb_raw);

# Discard first and last values
l = length(uwb_raw);
filtered_uwb = uwb_mvavg(wndw+1:l-wndw,:);

# Export Filtered
save(outmat, 'filtered_uwb');

# Plot uwb_raw
plot(uwb_raw, "r");
hold on;
plot(filtered_uwb, "c");
hold on;

# Set up Plot
grid on;
title('Distance to UWB Receiver');
xlabel('Sample Number');
ylabel('Distance (cm)');
legend('raw', 'filtered');
ylim([0 150]);

# Print Plot
print(outfile);