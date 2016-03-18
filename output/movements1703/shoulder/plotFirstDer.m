# Parameters
outfile = "data_simple_cal_first_der.pdf";
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
gravity = 9.80665;
f_ideal = [0; 0; 1];

# Read in calibrated data
load ('simple_calibration.mat');
gyr = cal(:,1:3);
acc = cal(:,4:6);

# Read in PWM data
data = load('data.txt');
pwm = data(:,12);

# Read in normal force
load('fnorm.mat', 'f_norm');

%  # Remove gravity
%  load('fnorm.mat', 'f_norm');
%  acc(1,:) = acc(1,:) - fnorm(1,1);
%  acc(2,:) = acc(2,:) - fnorm(2,1);
%  acc(3,:) = acc(3,:) - fnorm(3,1);

# Convert Data
gyr = gyr / gyr_sens;
acc = acc / acc_sens;
f_norm = f_norm / acc_sens;

# Convert Acc to m/s/s
acc = acc * gravity;

# Compute first derivative
v(1) = 0;
angle(1) = 0;
for i=1:length(gyr)
  
  # Get expected angle
  if (pwm(i) == 1167)
    angle_exp(i) = 30;
  elseif (pwm(i) == 833)
    angle_exp(i) = 60;
  elseif (pwm(i) == 500)
    angle_exp(i) = 90;
  elseif (pwm(i) == 1500)
    angle_exp(i) = 0;
  endif;

  # Get gyro-angle, acc-angle velocity and position
  for j = 1:3
    if (i == 1)
      angle(i,j) = 0;
      v(i,j) = 0;
      p(i,j) = 0;
    else
      angle(i,j) = angle(i-1,j) + gyr(i-1,j) * delta_t;
      v(i,j) = v(i-1,j) + acc(i,j) * delta_t;
      p(i,j) = p(i-1, j) + v(i-1, j) * delta_t;
    endif;
  endfor;
  
  # Compute Sum of accelerations
  accsum(i) = sqrt(acc(i,1)^2 + acc(i,2)^2 + acc(i,3)^2);

  # Get expected x and z accelerations
  ax_exp = -f_norm(3,1) * sin(angle_exp/180*pi); % negative x axis because gravity is pointing opposite of x axis if angle is positive
  az_exp = f_norm(3,1) * cos(angle_exp/180*pi);
  
  # Convert to m/s^2
  ax_exp_g = ax_exp*gravity;
  az_exp_g = az_exp*gravity;
  
  # Create expected acceleration vector
  acc_exp = [ax_exp(i); 0; az_exp(i)];
  
  # Normalize
  acc_exp_norm = acc_exp / norm(acc_exp);
  
  # Get Angle from acc_exp
  angle_acc_exp = acos(acc_exp(1,1)*f_ideal(1,1) + acc_exp(2,1)*f_ideal(2,1) + acc_exp(3,1)*f_ideal(1,1));
  
  # Is there any rotation?
  if (acc_exp_norm != f_ideal(1,1) || acc_exp_norm(2,1) != f_ideal(2,1) || acc_exp_norm(3,1) != f_ideal(3,1))
  
    # Get rotation axis vector
    rot_vec = cross(acc_exp_norm, f_ideal);
    
    # Normalize rotation axis vector
%      rot_vec_mag = norm(rot_vec);
%      if (rot_vec_mag != 0)
%        rot_vec = rot_vec / rot_vec_mag;
%      endif;  
  
    # Compute Quaternions from expected accelerations
    quat_abs_acc_exp = unit(quaternion(cos(angle_acc_exp/2), rot_vec(1,1)*sin(angle_acc_exp/2), rot_vec(2,1)*sin(angle_acc_exp/2), rot_vec(3,1)*sin(angle_acc_exp/2)));
%      quat_abs_acc_exp = rot2q(rot_vec, angle_acc_exp);
  else
    quat_abs_acc_exp = quaternion(1, 0, 0, 0);
  endif;
  
  # Remember for plot
  quat_acc_exp(i,1) = quat_abs_acc_exp.w;
  quat_acc_exp(i,2) = quat_abs_acc_exp.x;
  quat_acc_exp(i,3) = quat_abs_acc_exp.y;
  quat_acc_exp(i,4) = quat_abs_acc_exp.z;
endfor;  

# Plot Angular Velocity
plot(gyr(:,1), "r");
hold on;
plot(gyr(:,2), "g");
hold on;
plot(gyr(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Gyroscope: Rate');
xlabel('Sample Number');
ylabel('Angular Velocity (in dgr/s)');
legend('x', 'y', 'z');

# Print
print(outfile);
hold off;

# Plot Angle
plot(angle(:,1), "r");
hold on;
plot(angle(:,2), "g");
hold on;
plot(angle(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Gyroscope: Angle');
xlabel('Sample Number');
ylabel('Angle (in dgr)');
legend('x', 'y', 'z');

# Print
print("-append", outfile);
hold off;

# Plot y Angle and expected y angle
plot(angle(:,2), "g");
hold on;
plot(angle_exp, "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope: Angle around y-axis');
xlabel('Sample Number');
ylabel('Angle (in dgr)');
legend('Measured', 'Expected');

# Print
print("-append", outfile);
hold off;

# Plot Acceleration
plot(acc(:,1), "r");
hold on;
plot(acc(:,2), "g");
hold on;
plot(acc(:,3), "b");
hold on;
plot(accsum, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer: Acceleration');
xlabel('Sample Number');
ylabel('Acceleration (m/s/s)');
legend('x', 'y', 'z', 'sqrt(x^2 + y^2 + z^2)');

# Print
print("-append", outfile);
hold off;

# Plot x and z accelerations and expected values
plot(acc(:,1), "r");
hold on;
plot(acc(:,3), "b");
hold on;
plot(ax_exp_g, "m");
hold on;
plot(az_exp_g, "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer: Acceleration vs. Expected Acceleration');
xlabel('Sample Number');
ylabel('Acceleration (m/s/s)');
legend('x', 'z','Expected x', 'Expected z');

# Print
print("-append", outfile);
hold off;

# Plot Quaternion
plot(quat_acc_exp(:,1), "c");
hold on;
plot(quat_acc_exp(:,2), "r");
hold on;
plot(quat_acc_exp(:,3), "g");
hold on;
plot(quat_acc_exp(:,4), "b");
hold on;

# Set up Plot
grid on;
title('Quaternions (expected from accelerations)');
xlabel('Sample Number');
ylabel('Value');
legend('w', 'x', 'y', 'z');

# Print
print("-append", outfile);
hold off;

%  # Plot Velocity
%  plot(v(:,1), "r");
%  hold on;
%  plot(v(:,2), "g");
%  hold on;
%  plot(v(:,3), "b");
%  hold on;
%  
%  # Set up Plot
%  grid on;
%  title('Accelerometer: Velocity');
%  xlabel('Sample Number');
%  ylabel('Velocity (m/s)');
%  legend('x', 'y', 'z');
%  
%  # Print
%  print("-append", outfile);
%  hold off;
%  
%  # Plot Position
%  plot(p(:,1), "r");
%  hold on;
%  plot(p(:,2), "g");
%  hold on;
%  plot(p(:,3), "b");
%  hold on;
%  
%  # Set up Plot
%  grid on;
%  title('Accelerometer: Position');
%  xlabel('Sample Number');
%  ylabel('Position (m)');
%  legend('x', 'y', 'z');
%  
%  # Print
%  print("-append", outfile);
%  hold off;