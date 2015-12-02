/*
 * pwmsw.h
 * Author: Stephanie Kunkel
 */

#ifndef __PMWSW_H_
#define __PWMSW_H_

//Defines
#define BASE_REG 		PWM_S00_AXI_SLV_REG0_OFFSET
#define SHOULDER_REG 	PWM_S00_AXI_SLV_REG1_OFFSET
#define ELBOW_REG 		PWM_S00_AXI_SLV_REG2_OFFSET
#define WRIST_REG 		PWM_S00_AXI_SLV_REG3_OFFSET
#define FINGER_REG 		PWM_S00_AXI_SLV_REG4_OFFSET


//Functions
int pwm(int amount);

#endif
