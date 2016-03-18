# Load Signal Package
pkg load signal

# Parameters
filterid = 1; % 0 = mvgavg, 1 = fir
wndw = 20*500/20; %20 * freq_IMU / freq_UWB
val = 1/500;
name = "filteredUwb";


# Generate Filenames
outfile = strcat(name, ".pdf");
outmat = strcat(name, ".mat");

# Load UWB Data
data = load('data.txt');
uwb_raw = data(:,17);

# Filter
if (filterid == 0) %Compute Moving Average
  uwb_temp = filter(ones(1, wndw)/wndw, 1, uwb_raw);
  
elseif (filterid == 1) % FIR filter
  # FIR Filter Coefficients
  coeff = fir1(wndw, val, 'low');

  # Apply Filter
  uwb_temp = filter(coeff, 1, uwb_raw);
endif;

# Discard first and last values
l = length(uwb_raw);
filtered_uwb = uwb_temp(wndw/2+1:l-wndw/2);
filtered_uwb(1:wndw/2+1) = uwb_temp(wndw+1);
filtered_uwb(l-wndw:l) = uwb_temp(l-wndw);


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
%ylim([0 150]);

# Print Plot
print(outfile);