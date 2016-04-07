# Load Quaternion pkg
pkg load quaternion;

# Parameters
filter = 4; % 0 = "raw", 1 = "static cal", 2 = "simple cal + mvavg", 3 = "simple cal + fir", 4 = "simple cal + kalman", 5 = "simple cal + no filter"
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;

# Load Data
if (filter == 1)
	load ('calibrated.mat', 'cal');
	data = cal;
	filter_str = "cal";
	f_norm = [331; -263; 8082];
	grav_raw = 8082;
elseif (filter == 2)
	load('mvavg_simple_cal.mat', 'mvavg');
	data = mvavg;
	filter_str = "mvavg";
	load('fnorm.mat', 'f_norm');
elseif (filter ==  3)
	load('fir_simple_cal.mat', 'firfil');
	data = firfil;
	filter_str = "fir";
	load('fnorm.mat', 'f_norm');
elseif (filter == 4)
	load('kalman_simple_cal.mat', 'kalmanfil');
	data = kalmanfil;
	filter_str = "kalman";
	load('fnorm.mat', 'f_norm');
elseif (filter == 5)
	load('simple_calibration.mat', 'cal');
	data = cal;
	filter_str = "simple_cal";
	load('fnorm.mat', 'f_norm');
else
	data = load('data.txt');
	filter_str = "raw";
endif

# Create Outfile Strings
outfile = strcat("quat_", filter_str, "_noFusion");
q_mat_outfile = strcat("quat_", filter_str, "_noFusion.mat");
%avs_outfile = strcat("avs_", filter_str, "_noFusion.mat");

#Read in data and convert
gx = data(:,1) / gyr_sens;
gy = data(:,2) / gyr_sens;
gz = data(:,3) / gyr_sens;
ax = data(:,4) / acc_sens;
ay = data(:,5) / acc_sens;
az = data(:,6) / acc_sens;

# Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);

# Compute Quaternions
for i = 1:length(gx)
  
  #Get data set
  gyr = [gx(i); gy(i); gz(i)];
  acc = [ax(i); ay(i); az(i)];
  
  #Convert to radians
  gyr_rad = gyr * pi/180;
  
  # Normalize Gyr
  gyr_norm_sc = norm(gyr_rad);
  if (gyr_norm_sc != 0)
    gyr_norm = gyr_rad / gyr_norm_sc;
  else 
    gyr_norm_sc = 1;
  endif
  
  # Normalize Acc
  acc_norm_sc = acc / norm(acc);
  if (acc_norm_sc == 0)
    acc_norm_sc = 1;
  endif;
  
  # Integrate --> rotation angle
  angle = gyr_norm_sc * delta_t;
  
  # Construct Quaternion
  quat_rel = quaternion(cos(angle/2), gyr_norm(1,1)*sin(angle/2), gyr_norm(2,1)*sin(angle/2), gyr_norm(3,1)*sin(angle/2));
  
  # Compute Absolute rotation
  quat_abs = quat_abs * quat_rel;
  
  # Keep track of quaternions for plot
  ow(i) = quat_abs.w;
  ox(i) = quat_abs.x;
  oy(i) = quat_abs.y;
  oz(i) = quat_abs.z;
  
endfor

# Plot Quaternion
plot(ow, "c");
hold on;
plot(ox, "r");
hold on;
plot(oy, "g");
hold on;
plot(oz, "b");
hold on;

# Set up Plot
grid on;
title('Quaternions');
xlabel('Sample Number');
ylabel('Value');
legend('w', 'x', 'y', 'z');

# Prepare for Data Export
ow = rot90(ow, -1);
ox = rot90(ox, -1);
oy = rot90(oy, -1);
oz = rot90(oz, -1);
out = [ow ox oy oz];

# Data Export
save(q_mat_outfile, 'out');

# Print
print(strcat(outfile, '.pdf'));

# Print Min and Max
printf("Quaternion (Min/Max):\r\n");
printf("w %f / %f\r\n", min(ow), max(ow));
printf("x %f / %f\r\n", min(ox), max(ox));
printf("y %f / %f\r\n", min(oy), max(oy));
printf("z %f / %f\r\n", min(oz), max(oz));