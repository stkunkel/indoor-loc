# Parameters
order = 1;
pcs = 1;
outfile = "trend_compare.pdf";

# Read in raw data
data = load ('data.txt');
rgx = data(:,1);
rgy = data(:,2);
rgz = data(:,3);
rax = data(:,4);
ray = data(:,5);
raz = data(:,6);

# Read in calibrated data
load('calibrated.mat', 'cal');
cgx = cal(:,1);
cgy = cal(:,2);
cgz = cal(:,3);
cax = cal(:,4);
cay = cal(:,5);
caz = cal(:,6);

# Get samples
l = [1:length(rgx)];

# Get trend rgx
pprgx = splinefit(l, rgx, pcs, "order", order);
trgx = ppval(pprgx, l);

# Get trend rgy
pprgy = splinefit(l, rgy, pcs, "order", order);
trgy = ppval(pprgy, l);

# Get trend rgz
pprgz = splinefit(l, rgz, pcs, "order", order);
trgz = ppval(pprgz, l);

# Get trend rax
pprax = splinefit(l, rax, pcs, "order", order);
trax = ppval(pprax, l);

# Get trend ray
ppray = splinefit(l, ray, pcs, "order", order);
tray = ppval(ppray, l);

# Get trend raz
ppraz = splinefit(l, raz, pcs, "order", order);
traz = ppval(ppraz, l);

# Get trend cgx
ppcgx = splinefit(l, cgx, pcs, "order", order);
tcgx = ppval(ppcgx, l);

# Get trend cgy
ppcgy = splinefit(l, cgy, pcs, "order", order);
tcgy = ppval(ppcgy, l);

# Get trend cgz
ppcgz = splinefit(l, cgz, pcs, "order", order);
tcgz = ppval(ppcgz, l);

# Get trend cax
ppcax = splinefit(l, cax, pcs, "order", order);
tcax = ppval(ppcax, l);

# Get trend cay
ppcay = splinefit(l, cay, pcs, "order", order);
tcay = ppval(ppcay, l);

# Get trend caz
ppcaz = splinefit(l, caz, pcs, "order", order);
tcaz = ppval(ppcaz, l);

# Print statistics
printf("Gyroscope raw:\r\n");
printf("gx: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(rgx), median(rgx), min(rgx), max(rgx), range(rgx), std(rgx));
printf("gy: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(rgy), median(rgy), min(rgy), max(rgy), range(rgy), std(rgy));
printf("gz: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(rgz), median(rgz), min(rgz), max(rgz), range(rgz), std(rgz));
printf("Gyroscope calibrated:\r\n");
printf("gx: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(cgx), median(cgx), min(cgx), max(cgx), range(cgx), std(cgx));
printf("gy: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(cgy), median(cgy), min(cgy), max(cgy), range(cgy), std(cgy));
printf("gz: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(cgz), median(cgz), min(cgz), max(cgz), range(cgz), std(cgz));
printf("Accelerometer raw:\r\n");
printf("ax: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(rax), median(rax), min(rax), max(rax), range(rax), std(rax));
printf("ay: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(ray), median(ray), min(ray), max(ray), range(ray), std(ray));
printf("az: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(raz), median(raz), min(raz), max(raz), range(raz), std(raz));
printf("Accelerometer calibrated:\r\n");
printf("ax: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(cax), median(cax), min(cax), max(cax), range(cax), std(cax));
printf("ay: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(cay), median(cay), min(cay), max(cay), range(cay), std(cay));
printf("az: Mean: %f, Median: %f, Min: %f, Max: %f, Range: %f, Stddev: %f\r\n", mean(caz), median(caz), min(caz), max(caz), range(caz), std(caz));

# Plot Gyro x
plot(trgx/mean(trgx), "r");
hold on;
plot(tcgx/mean(tcgx), "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope Trend (x)');
xlabel('Sample Number');
ylabel('Value / Mean');
legend('raw', 'calibrated');
hold off;

# Print
print(outfile);

# Plot Gyro y
plot(trgy/mean(trgy), "g");
hold on;
plot(tcgy/mean(tcgy), "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope Trend (y)');
xlabel('Sample Number');
ylabel('Value / Mean');
legend('raw', 'calibrated');
hold off;

# Print
print("-append", outfile);

# Plot Gyro z
plot(trgz/mean(trgz), "g");
hold on;
plot(tcgz/mean(tcgz), "c");
hold on;

# Set up Plot
grid on;
title('Gyroscope Trend (z)');
xlabel('Sample Number');
ylabel('Value / Mean');
legend('raw', 'calibrated');
hold off;

# Print
print("-append", outfile);

# Plot Accel x
plot(trax/mean(trax), "r");
hold on;
plot(tcax/mean(tcax), "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer Trend (x)');
xlabel('Sample Number');
ylabel('Value / Mean');
legend('raw', 'calibrated');
hold off;

# Print
print("-append", outfile);

# Plot Accel y
plot(tray/mean(tray), "g");
hold on;
plot(tcay/mean(tcay), "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer Trend (y)');
xlabel('Sample Number');
ylabel('Value / Mean');
legend('raw', 'calibrated');
hold off;

# Print
print("-append", outfile);

# Plot Accel z
plot(traz/mean(traz), "g");
hold on;
plot(tcaz/mean(tcaz), "c");
hold on;

# Set up Plot
grid on;
title('Accelerometer Trend (z)');
xlabel('Sample Number');
ylabel('Value / Mean');
legend('raw', 'calibrated');
hold off;

# Print
print("-append", outfile);
