# Parameters
f_norm = [331; -263; 8082];
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
gravity = 9.80665;
v = [0; 0; 0];
s = [0; 0; 0];

# Load Calibrated Data
load ('mvavg.mat', 'mvavg');
ax = mvavg(:,4);
ay = mvavg(:,5);
az = mvavg(:,6);

# Load Quaternions (after calibration)
load('quat_cal_filt.mat', 'quat_cal_filt');
qw = quat_cal_filt(:,1);
qx = quat_cal_filt(:,2);
qy = quat_cal_filt(:,3);
qz = quat_cal_filt(:,4);

# Convert Normal Force to G
f_norm = f_norm / acc_sens;

# Compute Velocity and Position
for i = 1:length(ax)
  
  #Get data set
  acc_raw = [ax(i); ay(i); az(i)];
  
  #Convert to G
  acc = acc_raw / acc_sens;
  
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

# Plot Velocity
plot(vx, "r");
hold on;
plot(vy, "g");
hold on;
plot(vz, "b");
hold on;

# Set up Plot
grid on;
title('Velocity (after calibration and filtering)');
xlabel('Sample Number');
ylabel('Velocity (m/s^2)');
legend('x', 'y', 'z');
hold off;

# Print Velocity
print("velPos_cal_filt.pdf");

# Plot Position
plot(sx, "r");
hold on;
plot(sy, "g");
hold on;
plot(sz, "b");
hold on;

# Set up Plot
grid on;
title('Position (after calibration and filtering)');
xlabel('Sample Number');
ylabel('Position (m/s)');
legend('x', 'y', 'z');
hold off;

# Print Position
print("-append", "velPos_cal_filt.pdf");