# Load Quaternion pkg
pkg load quaternion;

# Parameters
filter = 3; % 0 = "raw", 1 = "static cal", 2 = "simple cal + mvavg", 3 = "simple cal + fir", 4 = "simple cal + kalman", 5 = "simple cal + no filter"
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
grav_raw = acc_sens;
gravity = 9.80665;
f_norm = [0; 0; acc_sens];
v = [0; 0; 0];
s = [0; 0; 0];

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
avs_outfile = strcat("avs_", filter_str, "_noFusion.mat");

#Read in data and convert
gx = data(:,1) / gyr_sens;
gy = data(:,2) / gyr_sens;
gz = data(:,3) / gyr_sens;
ax = data(:,4) / acc_sens;
ay = data(:,5) / acc_sens;
az = data(:,6) / acc_sens;

# Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);

# Convert Normal Force to G
f_norm = f_norm / acc_sens;

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
  
  ################ Velocity and Position ################

  
  #Helper Computations
  qw_qw = ow(i) * ow(i);
  qw_qx = ow(i) * ox(i);
  qw_qy = ow(i) * oy(i);
  qw_qz = ow(i) * oz(i);
  qx_qx = ox(i) * ox(i);
  qx_qy = ox(i) * oy(i);
  qx_qz = ox(i) * oz(i);
  qy_qy = oy(i) * oy(i);
  qy_qz = oy(i) * oz(i);
  qz_qz = oz(i) * oz(i);
  
  #Quaternion to Rotation Matrix
  rot = [(qw_qw + qx_qx - qy_qy - qz_qz) 	(2 * qx_qy + 2 * qw_qz) 	(2 * qx_qz - 2 * qw_qy); 
	  (2 * qx_qy - 2 * qw_qz) 		(qw_qw - qx_qx + qy_qy - qz_qz) (2 * qy_qz + 2 * qw_qx)
	  (2 * qx_qz + 2 * qw_qy) 		(2 * qy_qz - 2 * qw_qx) 	(qw_qw - qx_qx - qy_qy + qz_qz)];
  
  #Get Transpose
  rot_t = transpose(rot);
  
  #Compute Intertial Acceleration
  acc_i = rot_t * acc - f_norm;
  
  #Convert to m/s^2
  acc_i_c = acc_i * gravity;
  
  # Get current acceleration
  if ( i == 1 )
    a_curr = [0; 0; 0];
  else
    a_curr = acc_i_c;
  endif;
  
  #Compute Velocity (first integration)
  v_curr = v + a_curr * delta_t; %v + acc_i_c * delta_t;
  
  #Compute Position (second integration)
  s_curr = s + v_curr * delta_t; %s + v*delta_t + 0.5 * acc_i_c * delta_t^2;
  
  #Replace old velocity and position by new ones
  v = v_curr;
  s = s_curr;
  
  # Keep track of velocity and position for plots
  oax(i) = a_curr(1,1);
  oay(i) = a_curr(2,1);
  oaz(i) = a_curr(3,1);
  vx(i) = v(1,1);
  vy(i) = v(2,1);
  vz(i) = v(3,1);
  sx(i) = s(1,1);
  sy(i) = s(2,1);
  sz(i) = s(3,1);
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
hold off;

# Print
print(strcat(outfile, '.pdf'));

# Prepare for Data Export
qw = rot90(ow, -1);
qx = rot90(ox, -1);
qy = rot90(oy, -1);
qz = rot90(oz, -1);
oax = rot90(oax, -1);
oay = rot90(oay, -1);
oaz = rot90(oaz, -1);
vx = rot90(vx, -1);
vy = rot90(vy, -1);
vz = rot90(vz, -1);
sx = rot90(sx, -1);
sy = rot90(sy, -1);
sz = rot90(sz, -1);

# Data Export for IMU Viewer
out = [qw qx qy qz sx sy sz];
save(q_mat_outfile, 'out');

# Data Export for Fusion with UWB
avs = [oax oay oaz vx vy vz sx sy sz];
save(avs_outfile, 'avs');



# Plot Velocity
plot(vx, "r");
hold on;
plot(vy, "g");
hold on;
plot(vz, "b");
hold on;

# Set up Plot
grid on;
title('Velocity');
xlabel('Sample Number');
ylabel('Velocity (m/s)');
legend('x', 'y', 'z');
hold off;

# Print Velocity
print("-append", strcat(outfile, '.pdf'));

# Plot Position
plot(sx, "r");
hold on;
plot(sy, "g");
hold on;
plot(sz, "b");
hold on;

# Set up Plot
grid on;
title('Position');
xlabel('Sample Number');
ylabel('Position (m)');
legend('x', 'y', 'z');
hold off;

# Print Position
print("-append", strcat(outfile, '.pdf'));