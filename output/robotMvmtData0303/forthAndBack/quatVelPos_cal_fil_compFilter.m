# Load Quaternion pkg
pkg load quaternion;

# Parameters
filter = 2; % 0 = "raw", 1 = "cal", 2 = "mvavg", 3 = "fir", 4 = "kalman"
f_norm = [331; -263; 8082];
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
gravity = 9.80665;
gyro_weight = 0.98;
acc_range = 0.1;
grav_z = 8082/acc_sens;
v = [0; 0; 0];
s = [0; 0; 0];

# Load Data
if (filter == 1)
	load ('calibrated.mat', 'cal');
	data = cal;
	filter_str = "cal";
elseif (filter == 2)
	load('mvavg.mat', 'mvavg');
	data = mvavg;
	filter_str = "mvavg";
elseif (filter == "fir")
	load('fir.mat', 3);
	data = firfil;
	filter_str = "fir";
elseif (filter == 4)
	load('kalman.mat', 'kalmanfil');
	data = kalmanfil;
	filter_str = "kalman";
else
	data = load('data.txt');
	filter_str = "raw";
endif

# Create Outfile Strings
outfile = strcat("quat_posVel_", filter_str, "_compFilter.pdf");
q_mat_outfile = strcat("quatPos_", filter_str, "_compFilter.mat");

# Extract Gyro and Accel Data
gx = data(:,1);
gy = data(:,2);
gz = data(:,3);
ax = data(:,4);
ay = data(:,5);
az = data(:,6);

# Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);

# Convert Normal Force to G
f_norm = f_norm / acc_sens;

# Compute Quaternions
for i = 1:length(gx)
  
  #Get data set
  gyr_raw = [gx(i); gy(i); gz(i)];
  acc_raw = [ax(i); ay(i); az(i)];
  
  #Convert to dgr
  gyr = gyr_raw / gyr_sens;
  acc = acc_raw / acc_sens;
  
  ############## Quaternion Start ##############
  
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
  if (acc_norm_sc != 0)
	acc_norm = acc / acc_norm_sc;
  else 
    acc_norm_sc = 1;
  endif;
  
  # Integrate Gyro Data --> rotation angle
  angle_gyr = gyr_norm_sc * delta_t;
  
  # Get Angle from Acc
  angle_acc = acc_norm(3,1) / grav_z;
  
  # Construct Quaternion  from gyroscope
  quat_rel_gyr = quaternion(cos(angle_gyr/2), gyr_norm(1,1)*sin(angle_gyr/2), gyr_norm(2,1)*sin(angle_gyr/2), gyr_norm(3,1)*sin(angle_gyr/2));
  
  # Compute Absolute rotation quaternion from gyroscope
  quat_abs_gyr = quat_abs * quat_rel_gyr;
  
  # Get sum of accelerations
  acc_sum = acc(1,1) + acc(2,1) + acc(3,1);
  
  # IMU is sitting still (just gravity) --> Complementary Filter for rotation (combine gyro and acc)
  if (acc_sum < (grav_z + acc_range) && acc_sum > (grav_z - acc_range))

	# Construct absolute rotation quaternion from accelerometer
	quat_abs_acc = quaternion(cos(angle_acc/2), acc_norm(1,1)*sin(angle_acc/2), acc_norm(2,1)*sin(angle_acc/2), acc_norm(3,1)*sin(angle_acc/2));
	
	# Complementary filter
	w = gyro_weight * quat_abs_gyr.w + (1 - gyro_weight) * quat_abs_acc.w;
	x = gyro_weight * quat_abs_gyr.x + (1 - gyro_weight) * quat_abs_acc.x;
	y = gyro_weight * quat_abs_gyr.y + (1 - gyro_weight) * quat_abs_acc.y;
	z = gyro_weight * quat_abs_gyr.z + (1 - gyro_weight) * quat_abs_acc.z;
	
	# Create Quaternion
	quat_abs = quaternion(w, x, y, z);
	
  # IMU is moving --> use gyro rotation only
  else 
	quat_abs = quat_abs_gyr;
  endif;
  
  # Keep track of quaternions for plot
  qw(i) = quat_abs.w;
  qx(i) = quat_abs.x;
  qy(i) = quat_abs.y;
  qz(i) = quat_abs.z;
  
  ######## Velocity / Position Start #########
  
   #Helper Computations
  qw_qw = qw(i) * qw(i);
  qw_qx = qw(i) * qx(i);
  qw_qy = qw(i) * qy(i);
  qw_qz = qw(i) * qz(i);
  qx_qx = qx(i) * qx(i);
  qx_qy = qx(i) * qy(i);
  qx_qz = qx(i) * qz(i);
  qy_qy = qy(i) * qy(i);
  qy_qz = qy(i) * qz(i);
  qz_qz = qz(i) * qz(i);
  
  #Quaternion to Rotation Matrix
  rot = [(qw_qw + qx_qx - qy_qy - qz_qz) 	(2 * qx_qy + 2 * qw_qz) 	(2 * qx_qz - 2 * qw_qy); 
	  (2 * qx_qy - 2 * qw_qz) 		(qw_qw - qx_qx + qy_qy - qz_qz) (2 * qy_qz + 2 * qw_qx)
	  (2 * qx_qz + 2 * qw_qy) 		(2 * qy_qz - 2 * qw_qx) 	(qw_qw - qx_qx - qy_qy + qz_qz)];
  
  #Get Transpose
  rot_t = transpose(rot);
  
  #Rotate Normal force
  f_norm_rot = rot * f_norm;
  
  #Remove Gravity
  acc = acc - f_norm_rot;
  
  #Compute Intertial Acceleration
  acc_i = rot_t * acc;
  
  #Convert to m/s^2
  acc_i_c = acc_i * gravity;
  
  #Compute Velocity (first integration)
  v_curr = v + acc_i_c * delta_t;
  
  #Compute Position (second integration)
  s_curr = s + v*delta_t + 0.5 * acc_i_c * delta_t^2;
  
  #Replace old velocity and position by new ones
  v = v_curr;
  s = s_curr;
  
  # Keep track of velocity and position for plots
  vx(i) = v(1,1);
  vy(i) = v(2,1);
  vz(i) = v(3,1);
  sx(i) = s(1,1);
  sy(i) = s(2,1);
  sz(i) = s(3,1);
  
endfor

# clear
hold off;

# Plot Quaternion
plot(qw, "c");
hold on;
plot(qx, "r");
hold on;
plot(qy, "g");
hold on;
plot(qz, "b");
hold on;

# Set up Plot
grid on;
title('Quaternions (after calibration and filtering)');
xlabel('Sample Number');
ylabel('Value');
legend('w', 'x', 'y', 'z');

# Prepare for Data Export
qw = rot90(qw, -1);
qx = rot90(qx, -1);
qy = rot90(qy, -1);
qz = rot90(qz, -1);
sx = rot90(sx, -1);
sy = rot90(sy, -1);
sz = rot90(sz, -1);
out = [qw qx qy qz sx sy sz];

# Data Export
save(q_mat_outfile, 'out');

# Print
print(outfile);
hold off;

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
print("-append", outfile);

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
print("-append", outfile);