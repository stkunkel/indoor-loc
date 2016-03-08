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
  P_prev = P;
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

