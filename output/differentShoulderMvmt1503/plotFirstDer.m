# Parameters
outfile = "data_simple_cal_first_der.pdf";
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
gravity = 9.80665;

# Read in data
load ('simple_calibration.mat');
gyr = cal(:,1:3);
acc = cal(:,4:6);

%  # Remove gravity
%  load('fnorm.mat', 'f_norm');
%  acc(1,:) = acc(1,:) - fnorm(1,1);
%  acc(2,:) = acc(2,:) - fnorm(2,1);
%  acc(3,:) = acc(3,:) - fnorm(3,1);

# Convert Data
gyr = gyr / gyr_sens;
acc = acc / acc_sens;

# Convert Acc to m/s/s
acc = acc * gravity;

# Compute first derivative
v(1) = 0;
angle(1) = 0;
for i=1:length(gyr)
  for j = 1:3
    if (i == 1)
      angle(i,j) = 0;
      v(i,j) = 0;
      p(i,j) = 0;
    else
      angle(i,j) = angle(i-1,j) + gyr(i-1,j) * delta_t;
      v(i,j) = acc(i-1,j) * delta_t;
      p(i,j) = p(i-1, j) + v(i-1, j) * delta_t;
    endif;
  endfor;
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

# Plot Angule
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



# Plot Acceleration
plot(acc(:,1), "r");
hold on;
plot(acc(:,2), "g");
hold on;
plot(acc(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Accelerometer: Acceleration');
xlabel('Sample Number');
ylabel('Acceleration (m/s/s)');
legend('x', 'y', 'z');

# Print
print("-append", outfile);
hold off;

# Plot Velocity
plot(v(:,1), "r");
hold on;
plot(v(:,2), "g");
hold on;
plot(v(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Accelerometer: Velocity');
xlabel('Sample Number');
ylabel('Velocity (m/s)');
legend('x', 'y', 'z');

# Print
print("-append", outfile);
hold off;

# Plot Position
plot(p(:,1), "r");
hold on;
plot(p(:,2), "g");
hold on;
plot(p(:,3), "b");
hold on;

# Set up Plot
grid on;
title('Accelerometer: Position');
xlabel('Sample Number');
ylabel('Position (m)');
legend('x', 'y', 'z');

# Print
print("-append", outfile);
hold off;