%Read in quaternions
load ('quat.mat', 'out');
data = out;

% Y
y = data(:,3);
x = [1:length(y)];

% Get Peaks
y1 = abs(y(3:end-2));
y2 = abs(y(2:end-3));
y3 = abs(y(1:end-4));
y4 = abs(y(4:end-1));
y5 = abs(y(5:end));
idx = find(y1 > y3 & y1 > y5) + 1;

% Check whether there are peaks
if (length(idx) != 0)

  % Plot Data and Peaks
  plot(x, y, 'g', x(idx), y(idx), 'r.');
  hold on;

  % Set up Plot
  grid on;
  title('Quaternion');
  xlabel('Sample Number');
  ylabel('Value');
  legend('y', 'Peak (y)');

  % Print First Peak Value
  printf("First y Peak: %f\r\n", y(idx(1), 1));

  % Print
  print("quat_y.pdf");
  hold off;

endif

% Z
y = data(:,4);
x = [1:length(y)];

% Get Peaks
y1 = abs(y(3:end-2));
y2 = abs(y(2:end-3));
y3 = abs(y(1:end-4));
y4 = abs(y(4:end-1));
y5 = abs(y(5:end));
idx = find(y1 > y3 & y1 > y5) + 1;

% Check whether there are peaks
if (length(idx) != 0)

  % Plot Data and Peaks
  plot(x, y, 'b', x(idx), y(idx), 'r.');
  hold on;

  % Set up Plot
  grid on;
  title('Quaternion');
  xlabel('Sample Number');
  ylabel('Value');
  legend('z', 'Peak (z)');

  % Print First Peak Value
  printf("First z Peak: %f\r\n", y(idx(1), 1));

  % Print
  print("quat_z.pdf");
  hold off;

endif