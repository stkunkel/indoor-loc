/*
 * pwmsw.h
 * Author: Stephanie Kunkel
 */

#ifndef __PMWSW_H_
#define __PWMSW_H_

/*
 * Error Codes
 */
#define PWM_SUCCESS				0
#define PWM_FAILURE				1
#define PWM_REG_CONT_NO_MATCH	2

/*
 * Defines: General
 */
#define PWMADDRESS  	0x43c00000
#define PWMSTEPS		7
#define US_PER_DGR		22222 // 4s/°

/*
 * Defines: Registers
 */
#define PWM0_VAL_REG		PWM_S00_AXI_SLV_REG0_OFFSET
#define PWM1_VAL_REG		PWM_S00_AXI_SLV_REG1_OFFSET
#define PWM2_VAL_REG		PWM_S00_AXI_SLV_REG2_OFFSET
#define PWM3_VAL_REG		PWM_S00_AXI_SLV_REG3_OFFSET
#define PWM4_VAL_REG		PWM_S00_AXI_SLV_REG4_OFFSET
#define PWM5_VAL_REG		PWM_S00_AXI_SLV_REG5_OFFSET
#define PWM0_STEPS_REG		PWM_S00_AXI_SLV_REG6_OFFSET
#define PWM1_STEPS_REG		PWM_S00_AXI_SLV_REG7_OFFSET
#define PWM2_STEPS_REG		PWM_S00_AXI_SLV_REG8_OFFSET
#define PWM3_STEPS_REG		PWM_S00_AXI_SLV_REG9_OFFSET
#define PWM4_STEPS_REG		PWM_S00_AXI_SLV_REG10_OFFSET
#define PWM5_STEPS_REG		PWM_S00_AXI_SLV_REG11_OFFSET

/*
 * Readable Defines
 */
#define BASE_REG			PWM0_VAL_REG
#define SHOULDER_REG		PWM1_VAL_REG
#define ELBOW_REG			PWM2_VAL_REG
#define WRIST_REG			PWM3_VAL_REG
#define THUMB_REG			PWM4_VAL_REG
#define FINGER_REG			PWM5_VAL_REG

/*
 * Types
 */
typedef enum{
	base,
	shoulder,
	elbow,
	wrist,
	thumb,
	finger
} Joint;


/*
 * Functions
 */
int pwmTest();

#endif