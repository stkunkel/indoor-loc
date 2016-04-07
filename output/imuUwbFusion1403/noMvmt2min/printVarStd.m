load('simple_calibration.mat', 'cal');
printf("gx: %d %d\r\n", var(cal(:,1)), std(cal(:,1)));
printf("gy: %d %d\r\n", var(cal(:,2)), std(cal(:,2)));
printf("gz: %d %d\r\n", var(cal(:,3)), std(cal(:,3)));
printf("ax: %d %d\r\n", var(cal(:,4)), std(cal(:,4)));
printf("ay: %d %d\r\n", var(cal(:,5)), std(cal(:,5)));
printf("az: %d %d\r\n", var(cal(:,6)), std(cal(:,6)));