#ifndef I2C_UTILS_H_
#define I2C_UTILS_H_

//includes
#include "xiicps.h"

// PS I2C0 Init Parameters -> Zybo via MIO
#define IIC_DEVICE_ID  XPAR_PS7_I2C_0_DEVICE_ID
#define IIC_SCLK_RATE  100000//XPAR_PS7_I2C_0_I2C_CLK_FREQ_HZ

//Functions
int SetIiCSClk(XIicPs *InstancePtr, u32 FsclHz );
int iic_init(XIicPs *IicPs, u16 DeviceId, u32 ClkRate );
int iic_read2(XIicPs *IicPs, u8 Address, u8 Register, u8 *Data,
		int ByteCount);
int iic_write2(XIicPs *IicPs, u8 Address, u8 Register, u8 Data);

#endif /* I2C_UTILS_H_ */
