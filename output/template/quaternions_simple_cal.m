% Load Quaternion pkg
pkg load quaternion;

% Parameters
gyr_sens = 32.8;
acc_sens = 8192;
delta_t = 1/500;
n = 100; %number of samples for calibration

% Offset Removal Function
function out = removeOffset(data, n)
  
  % Get first n values
  part = data(1:n,:);
  
  % Compute Offset
  offset = mean(part);
  
  % Substract Offset
  out = data - offset;
  
  % Print Offset
  printf("Offset: %f\r\n", offset);

endfunction

%Read in data
data = load ('data.txt');
gxr = data(:,1);
gyr = data(:,2);
gzr = data(:,3);
axr = data(:,4);
ayr = data(:,5);
azr = data(:,6);

% Remove Offset
gx = removeOffset(gxr, n);
gy = removeOffset(gyr, n);
gz = removeOffset(gzr, n);
ax = removeOffset(axr, n);
ay = removeOffset(ayr, n);
az = removeOffset((azr-8192), n) + 8192;

% Absolute Quaternion
quat_abs = quaternion(1, 0, 0, 0);

% Convert degrees to radians
gxr = gx * pi/180;
gyr = gy * pi/180;
gzr = gz * pi/180;

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
  if (acc_norm_sc == 0)
    acc_norm_sc = 1;
  endif;
  
  % Integrate --> rotation angle
  angle = gyr_norm_sc * delta_t;
  
  % Construct Quaternion
  quat_rel = quaternion(cos(angle/2), gyr_norm(1,1)*sin(angle/2), gyr_norm(2,1)*sin(angle/2), gyr_norm(3,1)*sin(angle/2));
  
  % Compute Absolute rotation
  quat_abs = quat_abs * quat_rel;
  
  % Keep track of quaternions for plot
  ow(i) = quat_abs.w;
  ox(i) = quat_abs.x;
  oy(i) = quat_abs.y;
  oz(i) = quat_abs.z;
  
endfor

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
title('Quaternions');
xlabel('Sample Number');
ylabel('Value');
legend('w', 'x', 'y', 'z');

% Prepare for Data Export
ow = rot90(ow, -1);
ox = rot90(ox, -1);
oy = rot90(oy, -1);
oz = rot90(oz, -1);
out = [ow ox oy oz];

% Data Export
save('quat_simple_cal.mat', 'out');

% Print
print("quaternions_simple_cal.pdf");

% Print Min and Max
printf("Quaternion (Min/Max):\r\n");
printf("w %f / %f\r\n", min(ow), max(ow));
printf("x %f / %f\r\n", min(ox), max(ox));
printf("y %f / %f\r\n", min(oy), max(oy));
printf("z %f / %f\r\n", min(oz), max(oz));