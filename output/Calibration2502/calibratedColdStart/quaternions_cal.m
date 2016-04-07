# Load Quaternion pkg
pkg load quaternion;

# Parameters
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;

# Load Calibrated Data
load ('calibrated.mat', 'cal');
gx = cal(:,1);
gy = cal(:,2);
gz = cal(:,3);
ax = cal(:,4);
ay = cal(:,5);
az = cal(:,6);

# Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);

# Convert degrees to radians
gxr = gx * pi/180;
gyr = gy * pi/180;
gzr = gz * pi/180;

# Compute Quaternions
for i = 1:length(gx)
  
  #Get data set
  gyr_raw = [gx(i); gy(i); gz(i)];
  acc_raw = [ax(i); ay(i); az(i)];
  
  #Convert to dgr
  gyr = gyr_raw / gyr_sens;
  acc = acc_raw / acc_sens;
  
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
title('Quaternions (after calibration)');
xlabel('Sample Number');
ylabel('Value');
legend('w', 'x', 'y', 'z');

# Prepare for Data Export
ow = rot90(ow, -1);
ox = rot90(ox, -1);
oy = rot90(oy, -1);
oz = rot90(oz, -1);
quat_cal = [ow ox oy oz];

# Data Export
save('quat_cal.mat', 'quat_cal');

# Print
print("quaternions_cal.eps");