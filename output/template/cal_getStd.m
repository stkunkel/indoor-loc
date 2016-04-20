# Read in data
load ('calibrated.mat', 'cal');
gx = cal(:,1);
gy = cal(:,2);
gz = cal(:,3);
ax = cal(:,4);
ay = cal(:,5);
az = cal(:,6);

# Create Sub matrix of first 30 values
gx_small = gx(1:30,:);
gy_small = gy(1:30,:);
gz_small = gz(1:30,:);
ax_small = ax(1:30,:);
ay_small = ay(1:30,:);
az_small = az(1:30,:);

# Print mean of sub matrices
printf("STDEV Gyr: x: %f, y: %f, z: %f\r\n", std(gx_small), std(gy_small), std(gz_small));
printf("STDEV Acc: x: %f, y: %f, z: %f\r\n", std(ax_small), std(ay_small), std(az_small));