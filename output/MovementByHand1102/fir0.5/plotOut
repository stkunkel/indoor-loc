#Read in data
data = load ('out.txt');
gx = data(:,1);
gy = data(:,2);
gz = data(:,3);
ax = data(:,4);
ay = data(:,5);
az = data(:,6);
qw = data(:,7);
qx = data(:,8);
qy = data(:,9);
qz = data(:,10);
vx = data(:,11);
vy = data(:,12);
vz = data(:,13);
px = data(:,14);
py = data(:,15);
pz = data(:,16);

#Plot Gyro
subplot(4,1,1);
plot(gx, "r");
hold on;
plot(gy, "g");
hold on;
plot(gz, "b");
hold on;

#Set up Plot
grid on;
title('Gyroscope');
xlabel('Sample Number');
ylabel('Angular Velocity (in dgr/s)');
legend('gx', 'gy', 'gz');


#Plot Accel
subplot(5,1,2);
plot(ax, "r");
hold on;
plot(ay, "g");
hold on;
plot(az, "b");
hold on;

#Set up Plot
grid on;
title('Accelerometer');
xlabel('Sample Number');
ylabel('Acceleration (in g)');
legend('x', 'y', 'z');

#Plot Quat
subplot(5,1,3);
plot(qw, "m");
hold on;
plot(qx, "r");
hold on;
plot(qy, "g");
hold on;
plot(qz, "b");
hold on;

#Set up Plot
grid on;
title('Quaternions');
xlabel('Sample Number');
ylabel('Quaternion Rotation');
legend('w', 'x', 'y', 'z');

#Plot Vel
subplot(5,1,4);
plot(vx, "r");
hold on;
plot(vy, "g");
hold on;
plot(vz, "b");
hold on;

#Set up Plot
grid on;
title('Velocity');
xlabel('Sample Number');
ylabel('Velocity (in m/s)');
legend('x', 'y', 'z');

#Plot Pos
subplot(5,1,5);
plot(px, "r");
hold on;
plot(py, "g");
hold on;
plot(pz, "b");
hold on;

#Set up Plot
grid on;
title('Position');
xlabel('Sample Number');
ylabel('Position (in m)');
legend('x', 'y', 'z');

#Print
print("-r1200", "out.pdf");