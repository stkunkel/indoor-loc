#ifndef I2C_UTILS_H_
#define I2C_UTILS_H_

//includes
#include "xiicps.h"

// PS I2C0 Init Parameters -> Zybo via MIO
#define IIC_DEVICE_ID  XPAR_PS7_I2C_0_DEVICE_ID
#define IIC_SCLK_RATE  XPAR_PS7_I2C_0_I2C_CLK_FREQ_HZ

//Functions
int SetIiCSClk( volatile XIicPs *InstancePtr, u32 FsclHz );
int iic_init( volatile XIicPs *IicPs, u16 DeviceId, u32 ClkRate );


#endif /* I2C_UTILS_H_ */
