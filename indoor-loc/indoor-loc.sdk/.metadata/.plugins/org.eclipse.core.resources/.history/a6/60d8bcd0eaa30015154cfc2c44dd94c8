/*
 * iic_utils.h
 * Author: Stephanie Kunkel
 */

#ifndef I2C_UTILS_H_
#define I2C_UTILS_H_

//includes
#include <stdio.h>
#include <unistd.h>
#include "xparameters.h"
#include "xiicps.h"
#include "xtime_l.h"
#include "../print_utils.h"

// PS I2C0 Init Parameters
#define IIC_DEVICE_ID  	XPAR_PS7_I2C_0_DEVICE_ID
#define IIC_SCLK_RATE  	400000//100000//XPAR_PS7_I2C_0_I2C_CLK_FREQ_HZ
#define IIC_BASE_ADDR	XPAR_PS7_I2C_0_BASEADDR

//Functions
int SetIiCSClk(XIicPs *InstancePtr, u32 FsclHz );
int iic_init(XIicPs *IicPs, u16 DeviceId, u32 ClkRate);
int iic_reset_init_abort(XIicPs *IicPs, u16 DeviceId, u32 ClkRate);
int iic_burstWrite(XIicPs *IicPs, u8 Address, u8 Register, u32 length, unsigned char* Data);
int iic_read2(XIicPs *IicPs, u8 Address, u8 Register, u8 *Data,
		int ByteCount);

#endif /* I2C_UTILS_H_ */
