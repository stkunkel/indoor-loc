# Load Quaternion pkg
pkg load quaternion;

# Parameters
filter_in = 0; % 0 = "Complementary Filter", 1 = "Kalman Filter"
filter_out = 1; % 0 = "Complementary Filter", 1 = "Kalman Filter"
ign_samples = 0; % samples to ignore until Filter has converged
imu_weight = 0.98;
delta_t = 1/500;
n = 100;

#Kalman Function
function pos = kalman_pos(a_imu, v_imu, s_uwb, delta_t)
  
  # Standard Deviations
  var_a = var(a_imu);
  var_v = var(v_imu);
  var_s = var(s_uwb);


  # Initialize
  y_hat = [0; 0; 0];
  A = [1 (delta_t) 0; 0 0 0; 0 0 0]; 
  B = [0; delta_t; 1];
  H = [1 0 0];
  P = [1000 0 0; 0 1000 0; 0 0 1000];
  Q = [var_s 0 0; 0 var_v 0; 0 0 var_a];
  R = var_s;

  # Go through samples
  for i=1:length(a_imu)
  
    # Set u and z
    u = a_imu(i);
    z = s_uwb(i);
  
    # Rememver previous error covariance
    P_prev = P;

    # Prediction Update
    y_hat_neg = A*y_hat + B*u;
    P = A*P_prev*transpose(A) + Q;

    # Measurement Update
    K = (P_prev*transpose(H))*inverse(H*P_prev*transpose(H) + R);
    y_hat = y_hat_neg + K*(z - H*y_hat_neg);
    P = (eye(3,3) - K*H)*P_prev;
    
    # Store values
    pos(i,1) = y_hat(1,1);
  end
  
  disp(P);

endfunction

# Determine where to get data from
if (filter_in == 1)
  avs_infile = "avs_simple_cal_kalman.mat";
  filter_in_str = "kalman";
else %Complementary Filter
  avs_infile = "avs_simple_cal_compFilter.mat";
  filter_in_str = "compl";
endif;

# Create output strings
if (filter_out == 1)
  filter_out_str = "kalman";
  filter_out_outstr = "Kalman";
else
  filter_out_str = "compl";
  filter_out_outstr = "Complementary";
endif;

# Load Data from IMU
load(avs_infile, 'avs');
a_i = avs(:,1:3);
v_i = avs(:,4:6);
s_i = avs(:,7:9);

# Load Data from UWB
data = load('data.txt');%zeros(length(a_i), 3); % TODO
load('filteredUwb.mat', 'filtered_uwb');

# Get distance btw UWB receiver and sender (robot)
dist = mean(filtered_uwb(1:n));

# Remove offset from distance and convert to meters
s_u = (dist - filtered_uwb) / 100;

# Create Outfile Strings
outfile = strcat("posVel_", filter_in_str, "_", filter_out_str, ".pdf");
data_outfile = strcat("pos_", filter_in_str, "_", filter_out_str, ".mat");

# Compute Standard deviations
%stddev_imu = std([a_i(:,1); a_i(:,2); a_i(:,3)]);
stddev_uwb = std(s_u);%.000001;


if (filter_out == 1) % Kalman Filter for position
  pos_x = kalman_pos(a_i(:,1), v_i(:,1), s_u(:,1), delta_t);
  %pos_y = kalman_pos(a_i(:,2), v_i(:,2), s_u(:,2), stddev_imu, stddev_uwb, delta_t);
  %pos_z = kalman_pos(a_i(:,3), v_i(:,3), s_u(:,3), stddev_imu, stddev_uwb, delta_t);
else % Complementary Filter
  pos_x = imu_weight* s_i(:,1) + (1 - imu_weight) * s_u;
endif;

# Export Data
out = [pos_x]; %pos_y pos_z
save(data_outfile, 'out');

# Plot x
plot(s_i(:,1), "g");
hold on;
plot(s_u(:,1), "r");
hold on;
plot(pos_x, "b");
hold on;

# Set up Plot
grid on;
title(cstrcat('Sensor Fusion (IMU + UWB) using ', filter_out_outstr, ' Filter'));
xlabel('Sample Number');
ylabel('Position on x-axis (m)');
legend('IMU only', 'UWB only', filter_out_outstr);
ylim([-0.5 1]);

# Print
print(outfile);
hold off;

%  # Plot y
%  plot(s_i(:,2), "g");
%  hold on;
%  plot(s_u(:,2), "r");
%  hold on;
%  plot(pos_y, "b");
%  hold on;
%  
%  # Set up Plot
%  grid on;
%  title(cstrcat('Sensor Fusion (IMU + UWB) using ', filter_out_outstr, ' Filter'));
%  xlabel('Sample Number');
%  ylabel('Position on y-axis (m)');
%  legend('IMU only', 'UWB only', filter_out_outstr);
%  
%  # Print
%  print("-append", outfile);
%  hold off;
%  
%  plot(s_i(:,3), "g");
%  hold on;
%  plot(s_u(:,3), "r");
%  hold on;
%  plot(pos_z, "b");
%  hold on;
%  
%  # Set up Plot
%  grid on;
%  title(cstrcat('Sensor Fusion (IMU + UWB) using ', filter_out_outstr, ' Filter'));
%  xlabel('Sample Number');
%  ylabel('Position on z-axis (m)');
%  legend('IMU only', 'UWB only', filter_out_outstr);
%  
%  # Print
%  print("-append", outfile);
%  hold off;