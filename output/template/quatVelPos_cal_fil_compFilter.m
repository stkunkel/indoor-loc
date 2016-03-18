# Load Quaternion pkg
pkg load quaternion;

# Parameters
filter = 5; % 0 = "raw", 1 = "static cal", 2 = "static cal + mvavg", 3 = "static cal + fir", 4 = "static cal + kalman", 5 = "simple cal + no filter"
gyro_weight = 0.98;
acc_range = 0.01;
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
grav_raw = acc_sens;
gravity = 9.80665;
f_norm = [0; 0; acc_sens];
v = [0; 0; 0];
s = [0; 0; 0];
acc_cnt = 0;

# Load Data
if (filter == 1)
	load ('calibrated.mat', 'cal');
	data = cal;
	filter_str = "cal";
	f_norm = [331; -263; 8082];
	grav_raw = 8082;
elseif (filter == 2)
	load('mvavg.mat', 'mvavg');
	data = mvavg;
	filter_str = "mvavg";
	f_norm = [331; -263; 8082];
	grav_raw = 8082;
elseif (filter == "fir")
	load('fir.mat', 3);
	data = firfil;
	filter_str = "fir";
	f_norm = [331; -263; 8082];
	grav_raw = 8082;
elseif (filter == 4)
	load('kalman.mat', 'kalmanfil');
	data = kalmanfil;
	filter_str = "kalman";
	f_norm = [331; -263; 8082];
	grav_raw = 8082;
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
outfile = strcat("quat_posVel_", filter_str, "_compFilter.pdf");
q_mat_outfile = strcat("quatPos_", filter_str, "_compFilter.mat");
avs_outfile = strcat("avs_", filter_str, "_compFilter.mat");

# Extract Gyro and Accel Data and convert
gx = data(:,1) / gyr_sens;
gy = data(:,2) / gyr_sens;
gz = data(:,3) / gyr_sens;
ax = data(:,4) / acc_sens;
ay = data(:,5) / acc_sens;
az = data(:,6) / acc_sens;

# Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);
quat_abs_acc_rec = quat_abs;

# Convert Normal Force to G
f_norm = f_norm / acc_sens;

# Compute Gravity
grav_z = f_norm(3,1);

# Compute Quaternions
for i = 1:length(gx)
  
  #Get data set
  gyr = [gx(i); gy(i); gz(i)];
  acc = [ax(i); ay(i); az(i)];
  
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
  
  # Integrate Gyro Data --> rotation angle
  angle_gyr = gyr_norm_sc * delta_t;
  if(angle_gyr < 0)
    angle_gyr = 2*pi - angle_gyr;
  endif;
  
  # Construct Quaternion  from gyroscope
  quat_rel_gyr = quaternion(cos(angle_gyr/2), gyr_norm(1,1)*sin(angle_gyr/2), gyr_norm(2,1)*sin(angle_gyr/2), gyr_norm(3,1)*sin(angle_gyr/2));
  
  # Compute Absolute rotation quaternion from gyroscope
  quat_abs_gyr = quat_abs * quat_rel_gyr;
  
  # Get sum of accelerations
  acc_sum = sqrt(acc(1,1)^2 + acc(2,1)^2 + acc(3,1)^2);
  
  # IMU is sitting still (just gravity) --> Complementary Filter for rotation (combine gyro and acc)
  if (acc_sum < (grav_z + acc_range) && acc_sum > (grav_z - acc_range))
        
    # Normalize Acc
    acc_norm = acc / acc_sum;
    
    # Is IMUs z axis pointing towards global z axis?
    if (acc_norm != f_norm(1,1) || acc_norm(2,1) != f_norm(2,1) || acc_norm(3,1) != f_norm(3,1))
    
      # Get Rotation Angle
      angle_acc = acos(acc_norm(1,1)*f_norm(1,1) + acc_norm(2,1)*f_norm(2,1) + acc_norm(3,1)*f_norm(3,1));
      
      # Get rotation axis vector
      rot_vec = cross(acc_norm, f_norm);
      
      # Normalize rotation axis vector
      rot_vec_mag = norm(rot_vec);
      if (rot_vec_mag != 0)
	rot_vec = rot_vec / rot_vec_mag;
      endif;  

      # Construct absolute rotation quaternion from accelerometer
      quat_abs_acc = unit(quaternion(cos(angle_acc/2), rot_vec(1,1)*sin(angle_acc/2), rot_vec(2,1)*sin(angle_acc/2), rot_vec(3,1)*sin(angle_acc/2)));
    
    else
      quat_abs_acc = quaternion(1, 0, 0, 0);
    endif;
    
    # Increase Counter
    acc_cnt++;
    
    # Complementary filter
    w = gyro_weight * quat_abs_gyr.w + (1 - gyro_weight) * quat_abs_acc.w;
    x = gyro_weight * quat_abs_gyr.x + (1 - gyro_weight) * quat_abs_acc.x;
    y = gyro_weight * quat_abs_gyr.y + (1 - gyro_weight) * quat_abs_acc.y;
    z = gyro_weight * quat_abs_gyr.z + (1 - gyro_weight) * quat_abs_acc.z;
  
    # Create Quaternion
    quat_abs = quaternion(w, x, y, z);
	
  # IMU is moving --> use gyro rotation only
  else 
	quat_abs_acc = quat_abs_gyr;
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
  s_curr = s + v * delta_t; %s + v*delta_t + 0.5 * acc_i_c * delta_t^2;
  
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

# Print Acc cnt
printf("Acceleration used for angle: %d out of %d samples\r\n", acc_cnt, i);

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