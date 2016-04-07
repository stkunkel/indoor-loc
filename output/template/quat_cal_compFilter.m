% Load Quaternion pkg
pkg load quaternion;

% Parameters
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
gyro_weight = 0.98;
grav_z = 8082/acc_sens;

% Load Calibrated Data
load ('calibrated.mat', 'cal');
gx = cal(:,1);
gy = cal(:,2);
gz = cal(:,3);
ax = cal(:,4);
ay = cal(:,5);
az = cal(:,6);

% Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);

% Compute Quaternions
for i = 1:length(gx)
  
  %Get data set
  gyr_raw = [gx(i); gy(i); gz(i)];
  acc_raw = [ax(i); ay(i); az(i)];
  
  %Convert to dgr
  gyr = gyr_raw / gyr_sens;
  acc = acc_raw / acc_sens;
  
  %Convert to radians
  gyr_rad = gyr * pi/180;
  
  % Normalize Gyr
  gyr_norm_sc = norm(gyr_rad);
  if (gyr_norm_sc != 0)
    gyr_norm = gyr_rad / gyr_norm_sc;
  else 
    gyr_norm_sc = 1;
  endif
  
  % Normalize Acc
  acc_norm_sc = acc / norm(acc);
  if (acc_norm_sc != 0)
	acc_norm = acc / acc_norm_sc;
  else 
    acc_norm_sc = 1;
  endif;
  
  % Integrate Gyro Data --> rotation angle
  angle_gyr = gyr_norm_sc * delta_t;
  
  % Get Angle from Acc
  angle_acc = acc_norm(3,1) / grav_z;
  
  % Construct Quaternion  from gyroscope
  quat_rel_gyr = quaternion(cos(angle_gyr/2), gyr_norm(1,1)*sin(angle_gyr/2), gyr_norm(2,1)*sin(angle_gyr/2), gyr_norm(3,1)*sin(angle_gyr/2));
  
  % Compute Absolute rotation quaternion from gyroscope
  quat_abs_gyr = quat_abs * quat_rel_gyr;
  
   % Construct absolute rotation quaternion from accelerometer
  quat_abs_acc = quaternion(cos(angle_acc/2), acc_norm(1,1)*sin(angle_acc/2), acc_norm(2,1)*sin(angle_acc/2), acc_norm(3,1)*sin(angle_acc/2));
  
  % Complementary filter
  w = gyro_weight * quat_abs_gyr.w + (1 - gyro_weight) * quat_abs_acc.w;
  x = gyro_weight * quat_abs_gyr.x + (1 - gyro_weight) * quat_abs_acc.x;
  y = gyro_weight * quat_abs_gyr.y + (1 - gyro_weight) * quat_abs_acc.y;
  z = gyro_weight * quat_abs_gyr.z + (1 - gyro_weight) * quat_abs_acc.z;
  quat_abs = quaternion(w, x, y, z);
  
  % Keep track of quaternions for plot
  ow(i) = quat_abs.w;
  ox(i) = quat_abs.x;
  oy(i) = quat_abs.y;
  oz(i) = quat_abs.z;
  
endfor

% clear
hold off;

% Plot Quaternion
plot(ow, "c");
hold on;
plot(ox, "r");
hold on;
plot(oy, "g");
hold on;
plot(oz, "b");
hold on;

% Set up Plot
grid on;
title('Quaternions (after calibration)');
xlabel('Sample Number');
ylabel('Value');
legend('w', 'x', 'y', 'z');

% Prepare for Data Export
ow = rot90(ow, -1);
ox = rot90(ox, -1);
oy = rot90(oy, -1);
oz = rot90(oz, -1);
quat_cal_compFil = [ow ox oy oz];

% Data Export
save('quat_cal_compFil.mat', 'quat_cal_compFil');

% Print
print("quaternions_cal_compFil.pdf");