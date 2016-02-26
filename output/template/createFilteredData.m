# Load Signal Package
pkg load signal

# Read in data
data = load ('data.txt');
gx = data(:,1);
gy = data(:,2);
gz = data(:,3);
ax = data(:,4);
ay = data(:,5);
az = data(:,6);

# FIR Filter Coefficients
coeff = fir1(20, 0.1, 'low');

# Filter data
filtered_gx = filter(coeff, 1, gx);
filtered_gy = filter(coeff, 1, gy);
filtered_gz = filter(coeff, 1, gz);
filtered_ax = filter(coeff, 1, ax);
filtered_ay = filter(coeff, 1, ay);
filtered_az = filter(coeff, 1, az);

# Export Filtered Data
out = [filtered_gx filtered_gy filtered_gz filtered_ax filtered_ay filtered_az];
out = int16(out);
#save('-text', 'filtered.txt', 'out');

fid = fopen('filtered.txt', 'w');
fdisp(fid, out);
fclose(fid) 