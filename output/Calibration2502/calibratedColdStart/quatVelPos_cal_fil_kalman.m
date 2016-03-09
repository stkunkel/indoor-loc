# Load Quaternion pkg
pkg load quaternion;

# Parameters
filter = 1; % 0 = "raw", 1 = "cal", 2 = "mvavg", 3 = "fir", 4 = "kalman"
ign_samples = 0; % samples to ignore until Filter has converged
f_norm = [331; -263; 8082];
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
gravity = 9.80665;
gyro_weight = 0.98;
acc_range = 0.1;
f_norm_conv = [331; -263; 8082] /acc_sens;
stddev_rate_x = 24.500152 / gyr_sens;
stddev_rate_y = 1.029194 / gyr_sens;
stddev_rate_z = 3.045021 / gyr_sens;
stddev_angle_x = 22.443588 / acc_sens;
stddev_angle_y = 72.858551 / acc_sens;
stddev_angle_z = 40.575413 / acc_sens;
acc_rec = [0; 0; 1];
v = [0; 0; 0];
s = [0; 0; 0];

#Kalman Function
function angle_out = kalman_angle(angle_in, rate, stddev_angle, stddev_rate, delta_t)
  
  # Compute Variances
  var_angle = stddev_angle^2;
  var_rate = stddev_rate^2;

  # Initialize
  y_hat = [0; 0];
  A = [1 (-delta_t); 0 1];
  B = [delta_t; 0];
  H = eye(2,2);
  P = [1000 0; 0 1000];
  Q = [var_angle 0; 0 var_rate];
  R = [stddev_angle 0; 0 stddev_rate];

  # Go through samples
  for i=1:length(angle_in)
  
    # Rememver previous error covariance
    P_prev = P;

    # Prediction Update
    y_hat_neg = A*y_hat + B*rate(i);
    P = A*P_prev*transpose(A) + Q;

    # Measurement Update
    K = (P_prev*transpose(H))*inverse(H*P_prev*transpose(H) + R);
    y_hat = y_hat_neg + K*([angle_in(i); rate(i)] - H*y_hat_neg);
    P = (eye(2,2) - K*H)*P_prev;
    
    # Store values
    angle_out(i) = y_hat(1,1);
  end
  
  disp(P);

endfunction

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
outfile = strcat("quat_posVel_", filter_str, "_kalman.pdf");
q_mat_outfile = strcat("quatPos_", filter_str, "_kalman.mat");

# Extract Gyro and Accel Data
gx = data(:,1);
gy = data(:,2);
gz = data(:,3);
ax = data(:,4);
ay = data(:,5);
az = data(:,6);

# Convert
gx = gx / gyr_sens;
gy = gy / gyr_sens;
gz = gz / gyr_sens;
ax = ax / acc_sens;
ay = ay / acc_sens;
az = az / acc_sens;


# Compute Angle
for i = 1:length(gx)
  
  # Get Magnitude of acceleration
  acc_i = [ax(i); ay(i); az(i)];
  acc_mag = norm(acc_i);
  
  # Use recent Acc if acceleration is way different than gravity
  if (acc_mag < (1-acc_range) || acc_mag > (1+acc_range))
    acc_norm = acc_rec;
  elseif (acc_mag != 0) % normalize
    acc_norm = acc_i / acc_mag;
  else 
    acc_norm = acc_i;
  endif;
  
  # Store Acc for next iteration
  acc_rec = acc_norm;
  
  # Compute Angle from Accelerometer and convert to dgr
  angle_acc_x(i) = asin(acc_norm(1,1)) / pi * 180;
  angle_acc_y(i) = asin(acc_norm(2,1)) / pi * 180;
  angle_acc_z(i) = acos(acc_norm(3,1)) / pi * 180;
  
  # Compute Angle from Gyroscope
  if (i == 1)
    angle_gyr_x(i) = gx(i) * delta_t;
    angle_gyr_y(i) = gy(i) * delta_t;
    angle_gyr_z(i) = gz(i) * delta_t;
  else
    angle_gyr_x(i) = angle_gyr_x(i-1) + gx(i) * delta_t;
    angle_gyr_y(i) = angle_gyr_y(i-1) + gy(i) * delta_t;
    angle_gyr_z(i) = angle_gyr_z(i-1) + gz(i) * delta_t;
  endif;

endfor

# Kalman Filter for x axis
angle_x = kalman_angle(angle_acc_x, gx, stddev_angle_x, stddev_rate_x, delta_t);
angle_y = kalman_angle(angle_acc_y, gy, stddev_angle_y, stddev_rate_y, delta_t);
angle_z = kalman_angle(angle_acc_z, gz, stddev_angle_z, stddev_rate_z, delta_t);

# Plot x
plot(angle_acc_x, "g");
hold on;
plot(angle_gyr_x, "r");
hold on;
plot(angle_x, "b");
hold on;

# Set up Plot
grid on;
title('Sensor Fusion using Kalman Filter');
xlabel('Sample Number');
ylabel('Rotation around x-axis (dgr)');
legend('Accel only', 'Gyro only', 'Kalman');

# Print
print(outfile);
hold off;

# Plot y
plot(angle_acc_y, "g");
hold on;
plot(angle_gyr_y, "r");
hold on;
plot(angle_y, "b");
hold on;

# Set up Plot
grid on;
title('Sensor Fusion using Kalman Filter');
xlabel('Sample Number');
ylabel('Rotation around y-axis (dgr)');
legend('Accel only', 'Gyro only', 'Kalman');

# Print
print("-append", outfile);
hold off;

# Plot z
plot(angle_acc_z, "g");
hold on;
plot(angle_gyr_z, "r");
hold on;
plot(angle_z, "b");
hold on;

# Set up Plot
grid on;
title('Sensor Fusion using Kalman Filter');
xlabel('Sample Number');
ylabel('Rotation around z-axis (dgr)');
legend('Accel only', 'Gyro only', 'Kalman');

# Print
print("-append", outfile);
hold off;

# Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);

# Compute Quaternions
for i = (ign_samples+1):length(gx)
  
  #Get data set (already converted)
  gyr = [gx(i); gy(i); gz(i)];
  acc = [ax(i); ay(i); az(i)];
  
  ############## Quaternion Start ##############
  
  #Convert angle to radians
  angle_acc_z_rad = angle_acc_z * pi/180;
  
  # Normalize Acc
  acc_norm_sc = acc / norm(acc);
  if (acc_norm_sc != 0)
    acc_norm = acc / acc_norm_sc;
  else 
    acc_norm_sc = 1;
    acc_norm = acc;
  endif;
  
  # Construct absolute rotation quaternion
  quat_abs = quaternion(cos(angle_acc_z_rad(i)/2), acc_norm(1,1)*sin(angle_acc_z_rad(i)/2), acc_norm(2,1)*sin(angle_acc_z_rad(i)/2), acc_norm(3,1)*sin(angle_acc_z_rad(i)/2));
  
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
  f_norm_rot = rot * f_norm_conv;
  
  # Debug
  %keyboard();
  
  #Remove Gravity
  acc = acc - f_norm_rot;
  
  #Compute Intertial Acceleration
  acc_inert = rot_t * acc;
  
  #Convert to m/s^2
  acc_i_c = acc_inert * gravity;
  
  # Get recent acceleration
  if ( i <= (ign_samples+1) )
    a_recent = [0; 0; 0];
  else
    a_recent = [oax(i-1); oay(i-1); oaz(i-1)];
  endif;
  
  #Compute Velocity (first integration)
  v_curr = v + a_recent * delta_t; %v + acc_i_c * delta_t;
  
  #Compute Position (second integration)
  s_curr = s + v * delta_t; %s + v*delta_t + 0.5 * acc_i_c * delta_t^2;
  
  #Replace old velocity and position by new ones
  v = v_curr;
  s = s_curr;
  
  # Keep track of velocity and position for plots
  oax(i) = acc_i_c(1,1);
  oay(i) = acc_i_c(2,1);
  oaz(i) = acc_i_c(3,1);
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
print("-append", outfile);
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