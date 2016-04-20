% Load Quaternion pkg
pkg load quaternion;

% Parameters
filter_in = 2; % 0 = "Complementary Filter", 1 = "Kalman Filter", 2 = simple cal
filter_out = 1; % 0 = "Complementary Filter", 1 = "Kalman Filter"
ign_samples = 0; % samples to ignore until Filter has converged
imu_weight = 0.2;
delta_t = 1/500;
n = 100;

%Kalman Function
function pos = kalman_pos(a_imu, v_imu, s_uwb, delta_t)
  
  % Variances
  var_a = var(a_imu)
  var_v = var(v_imu)
  var_s = var(s_uwb)
  
  std_a = std(a_imu)
  std_v = std(v_imu)
  std_s = std(s_uwb)


  % Initialize
  y_hat = [0; 0; 0];
  A = [1 delta_t 0; 0 1 0; 0 0 0]; 
  B = [delta_t^2/2; delta_t; 1];
  H = [1 0 0];
  P = [1000 0 0; 0 1000 0; 0 0 1000];
  Q = [var_s 0 0; 0 var_v 0; 0 0 var_a];
  R = std_s;

  % Go through samples
  for i=1:length(a_imu)
  
    % Set u and z
    u = a_imu(i);
    z = s_uwb(i);
  
    % Rememver previous error covariance
    P_prev = P;

    % Prediction Update
    y_hat_neg = A*y_hat + B*u;
    P_neg = A*P_prev*transpose(A) + Q;

    % Measurement Update
    K = (P_neg*transpose(H))*inverse(H*P_neg*transpose(H) + R);
    y_hat = y_hat_neg + K*(z - H*y_hat_neg);
    P = (eye(3,3) - K*H)*P_neg;
    
    % Store values
    pos(i,1) = y_hat(1,1);
  end
  
  % Print Values
  printf("P:");
  disp(P);
  printf("\r\nQ:");
  disp(Q);
  printf("\r\nK:");
  disp(K);
  printf("\r\n\r\n");

endfunction

% Determine where to get data from
if (filter_in == 1) %Kalman Filter
  avs_infile = "avs_simple_cal_kalman.mat";
  filter_in_str = "kalman";
  
elseif (filter_in == 0) %Complementary Filter
  avs_infile = "avs_simple_cal_fir_hl_compFilter";
  filter_in_str = "compl";
  
else %simple cal
	avs_infile = "avs_simple_cal_noFusion.mat";
  filter_in_str = "simple_cal";
endif;

% Create outfile string
if (filter_out == 0)  %Complementary Filter
	filter_out_outstr = "Complementary";
	filter_out_str = "compl";
elseif (filter_out == 1) %Kalman Filter
	filter_out_outstr = "Kalman";
	filter_out_str = "kalman";
endif;

% Load Data from IMU
load(avs_infile, 'avs');
a_i = avs(:,1:3)*100;
v_i = avs(:,4:6)*100;
s_i = avs(:,7:9)*100;

% Load Data from UWB
%data = load('data.txt');%zeros(length(a_i), 3); % TODO
load('filteredUwb.mat', 'filtered_uwb');

% Get distance btw UWB receiver and sender (robot)
dist = mean(filtered_uwb(1:n));

% Remove offset from distance and convert to meters
s_u = (dist - filtered_uwb);

% Create Outfile Strings
outfile = strcat("posVel_", filter_in_str, "_", filter_out_str);
data_outfile = strcat("pos_", filter_in_str, "_", filter_out_str, ".mat");

% Filter
if (filter_out == 1) % Kalman Filter for position
  pos_x = kalman_pos(a_i(:,1), v_i(:,1), s_u(:,1), delta_t);
else % Complementary Filter
  pos_x = imu_weight* s_i(:,1) + (1 - imu_weight) * s_u;
endif;

% Export Data
out = [pos_x]; %pos_y pos_z
save(data_outfile, 'out');

% Plot x
plot(s_i(1:9750,1), "g");
hold on;
plot(s_u(1:9750,1), "r");
hold on;
plot(pos_x(1:9750), "b");
hold on;

% Set up Plot
grid on;
title(cstrcat('Sensor Fusion (IMU + UWB) using ', filter_out_outstr, ' Filter'));
xlabel('Sample Number');
ylabel('Position on x-axis (cm)');
legend('IMU only', 'UWB only', filter_out_outstr);
%ylim([-0.5 1]);

% Print
print("-color", strcat(outfile, ".eps"));
hold off;

%  % Plot y
%  plot(s_i(:,2), "g");
%  hold on;
%  plot(s_u(:,2), "r");
%  hold on;
%  plot(pos_y, "b");
%  hold on;
%  
%  % Set up Plot
%  grid on;
%  title(cstrcat('Sensor Fusion (IMU + UWB) using ', filter_out_outstr, ' Filter'));
%  xlabel('Sample Number');
%  ylabel('Position on y-axis (m)');
%  legend('IMU only', 'UWB only', filter_out_outstr);
%  
%  % Print
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
%  % Set up Plot
%  grid on;
%  title(cstrcat('Sensor Fusion (IMU + UWB) using ', filter_out_outstr, ' Filter'));
%  xlabel('Sample Number');
%  ylabel('Position on z-axis (m)');
%  legend('IMU only', 'UWB only', filter_out_outstr);
%  
%  % Print
%  print("-append", outfile);
%  hold off;