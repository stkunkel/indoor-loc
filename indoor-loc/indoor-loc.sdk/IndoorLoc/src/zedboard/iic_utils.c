/*
 * iic_utils.c: Utilities for I2C Communication.
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include "iic_utils.h"

/*
 * Functions
 */

/*
 * Set IIC Clock
 */
int SetIiCSClk(XIicPs *InstancePtr, u32 FsclHz) {
	u32 Div_a;
	u32 Div_b;
	u32 ActualFscl;
	u32 Temp;
	u32 TempLimit;
	u32 LastError;
	u32 BestError;
	u32 CurrentError;
	u32 ControlReg;
	u32 CalcDivA;
	u32 CalcDivB;
	u32 BestDivA = 0;
	u32 BestDivB = 0;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(FsclHz > 0);

//	if (0 != XIicPs_In32((InstancePtr->Config.BaseAddress) +
//					XIICPS_TRANS_SIZE_OFFSET)) {
//		return XST_DEVICE_IS_STARTED;
//	}

	/*
	 * Assume Div_a is 0 and calculate (divisor_a+1) x (divisor_b+1).
	 */
	Temp = (InstancePtr->Config.InputClockHz) / (22 * FsclHz);

	/*
	 * If the answer is negative or 0, the Fscl input is out of range.
	 */
	if (0 == Temp) {
		return XST_FAILURE;
	}

	/*
	 * If frequency 400KHz is selected, 384.6KHz should be set.
	 * If frequency 100KHz is selected, 90KHz should be set.
	 * This is due to a hardware limitation.
	 */
	if (FsclHz > 384600) {
		FsclHz = 384600;
	}

	if ((FsclHz <= 100000) && (FsclHz > 90000)) {
		FsclHz = 90000;
	}

	/*
	 * TempLimit helps in iterating over the consecutive value of Temp to
	 * find the closest clock rate achievable with divisors.
	 * Iterate over the next value only if fractional part is involved.
	 */
	TempLimit =
			((InstancePtr->Config.InputClockHz) % (22 * FsclHz)) ?
					Temp + 1 : Temp;
	BestError = FsclHz;

	for (; Temp <= TempLimit; Temp++) {
		LastError = FsclHz;
		CalcDivA = 0;
		CalcDivB = 0;
		CurrentError = 0;

		for (Div_b = 0; Div_b < 64; Div_b++) {

			Div_a = Temp / (Div_b + 1);

			if (Div_a != 0)
				Div_a = Div_a - 1;

			if (Div_a > 3)
				continue;

			ActualFscl = (InstancePtr->Config.InputClockHz)
					/ (22 * (Div_a + 1) * (Div_b + 1));

			if (ActualFscl > FsclHz)
				CurrentError = (ActualFscl - FsclHz);
			else
				CurrentError = (FsclHz - ActualFscl);

			if (LastError > CurrentError) {
				CalcDivA = Div_a;
				CalcDivB = Div_b;
				LastError = CurrentError;
			}
		}

		/*
		 * Used to capture the best divisors.
		 */
		if (LastError < BestError) {
			BestError = LastError;
			BestDivA = CalcDivA;
			BestDivB = CalcDivB;
		}
	}

	/*
	 * Read the control register and mask the Divisors.
	 */
	ControlReg = XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
			XIICPS_CR_OFFSET);
	ControlReg &= ~(XIICPS_CR_DIV_A_MASK | XIICPS_CR_DIV_B_MASK);
	ControlReg |= (BestDivA << XIICPS_CR_DIV_A_SHIFT)
			| (BestDivB << XIICPS_CR_DIV_B_SHIFT);

	XIicPs_WriteReg(InstancePtr->Config.BaseAddress, XIICPS_CR_OFFSET,
			ControlReg);

	return XST_SUCCESS;
}

/*
 * IIC Init
 */
int iic_init(XIicPs *IicPs, u16 DeviceId, u32 ClkRate) {
	int Status;
	XIicPs_Config *IicPs_Config;

	XIicPs_DisableAllInterrupts(IicPs->Config.BaseAddress);

	/*
	 * Initialize the IIC driver.
	 */
	IicPs_Config = XIicPs_LookupConfig(DeviceId);
	if (IicPs_Config == NULL) {
		myprintf("No XIicPs instance found for ID %d\n\r", DeviceId);
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(IicPs, IicPs_Config,
			IicPs_Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		myprintf("XIicPs Initialization failed for ID %d\n\r", DeviceId);
		return XST_FAILURE;
	}

	/*
	 * Set the IIC serial clock rate.
	 */
	//Status = XIicPs_SetSClk(IicPs, ClkRate);
	Status = SetIiCSClk(IicPs, ClkRate);
	if (Status != XST_SUCCESS) {
		myprintf("Setting XIicPs clock rate failed for ID %d, Status: %d\n\r",
				DeviceId, Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*
 * IIC Abort Transaction
 */
int iic_reset_init_abort(XIicPs *IicPs, u16 DeviceId, u32 ClkRate) {
	//Variables
//	int i = 0;

//	//Check if IIC is busy
//	while (XIicPs_BusIsBusy(IicPs)) {
//		i++;
//		if (i >= 1000) {
//#ifdef DEBUG
//			myprintf("Cannot abort IIC Transaction because IIC is busy.\r\n");
//#endif
//			return XST_FAILURE;
//		}
//	}

//Reset and Init
//	XIicPs_ResetHw(IIC_BASE_ADDR);
	iic_init(IicPs, IIC_DEVICE_ID, IIC_SCLK_RATE);
//	XIicPs_Reset(IicPs);

//Abort and return
	XIicPs_Abort(IicPs);
	return XST_SUCCESS;
}

/*
 * IIC Busy?
 */
int isIicBusBusy(XIicPs* IicPs) {
	u32 StatusReg = 0;

	StatusReg = XIicPs_ReadReg(IicPs->Config.BaseAddress, XIICPS_SR_OFFSET);
	if (StatusReg & XIICPS_SR_BA_MASK) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
 * I2C Write Functions
 */
/*
 * IIC Write 1
 */
int iic_write1(XIicPs *IicPs, u8 Address, u8 Data) {
	int Status;

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(IicPs)) {
		/* NOP */
	}

	/*
	 * Send the buffer using the IIC and check for errors.
	 */
	Status = XIicPs_MasterSendPolled(IicPs, &Data, 1, Address);
	if (Status != XST_SUCCESS) {
		myprintf("XIicPs_MasterSendPolled error!\n\r");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*
 * IIC Write 2
 */
int iic_write2(XIicPs *IicPs, u8 Address, u8 Register, u8 Data) {
	u8 WriteBuffer[2];
	int Status;

	/*
	 * A temporary write buffer must be used which contains both the address
	 * and the data to be written, put the address in first
	 */
	WriteBuffer[0] = Register;
	WriteBuffer[1] = Data;

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(IicPs)) {
		/* NOP */
		//sleep(1);
	}

	/*
	 * Send the buffer using the IIC and check for errors.
	 */
	Status = XIicPs_MasterSendPolled(IicPs, WriteBuffer, 2, Address);
	if (Status != XST_SUCCESS) {
		myprintf("XIicPs_MasterSendPolled error!\n\r");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*
 * IIC Burst Write
 */
int iic_burstWrite(XIicPs *IicPs, u8 Address, u8 Register, u32 length,
		unsigned char* Data) {
	u8 WriteBuffer[length + 1];
	int Status;
//	int cnt = 0;

	/*
	 * A temporary write buffer must be used which contains both the address
	 * and the data to be written, put the address in first
	 */
	WriteBuffer[0] = Register;
	memcpy(&WriteBuffer[1], Data, length);

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(IicPs)) {
		/* NOP */
		usleep(100);
//		cnt++;
//		if (cnt > 100) {
//			return XST_FAILURE;
//		}
	}

	/*
	 * Send the buffer using the IIC and check for errors.
	 */
	Status = XIicPs_MasterSendPolled(IicPs, WriteBuffer, length + 1, Address);
	if (Status != XST_SUCCESS) {
#ifdef DEBUG
		//Get Timestamp
		unsigned long timestamp_ms = getElapsedRuntimeUS()/1000;

		myprintf("XIicPs_MasterSendPolled error at %ds!\n\r", timestamp_ms);
#endif
		return XST_FAILURE;
	}
//print and return
//myprintf("[iic_burstWrite] 0x%02X(0x%02X)=0x%X\n\r", Address, Register, *Data);
	return XST_SUCCESS;
}

///*
// * IIC Write X
// */
//static void iic_writex(XIicPs *IicPs, u8 Address, ZED_I2C_CONFIG Config[],
//		u32 Length) {
//	int i;
//
//	for (i = 0; i < Length; i++) {
//		iic_write2(IicPs, Address, Config[i].Reg, Config[i].Init);
//	}
//}
//
///*
// * IIC Write X 2
// */
//static void iic_writex2(XIicPs *IicPs, ZED_I2C_CONFIG2 Config[], u32 Length) {
//	int i;
//
//	for (i = 0; i < Length; i++) {
//		iic_write2(IicPs, Config[i].Addr, Config[i].Reg, Config[i].Init);
//	}
//}

/*
 * I2C Read Functions
 */
/*
 * IIC Read 1
 */
int iic_read1(XIicPs *IicPs, u8 Address, u8 *Data) {
	int Status;

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(IicPs)) {
		/* NOP */
	}

	/*
	 * Receive the data.
	 */
	Status = XIicPs_MasterRecvPolled(IicPs, Data, 1, Address);
	if (Status != XST_SUCCESS) {
		myprintf("XIicPs_MasterRecvPolled error!\n\r");
		return XST_FAILURE;
	}
//	myprintf("[iic_read1] 0x%02X=0x%02X\n\r", Address, *Data);

	return XST_SUCCESS;
}

/*
 * IIC Read 2
 */
int iic_read2(XIicPs *IicPs, u8 Address, u8 Register, u8 *Data, int ByteCount) {
	int Status;
//	int cnt = 0;

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(IicPs)) {
		usleep(100);
//		cnt++;
//		if (cnt > 100) {
//			return XST_FAILURE;
//		}
	}

	/*
	 * Set the IIC Repeated Start option.
	 */
	Status = XIicPs_SetOptions(IicPs, XIICPS_REP_START_OPTION);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Send the buffer using the IIC and check for errors.
	 */
	Status = XIicPs_MasterSendPolled(IicPs, &Register, 1, Address);
	if (Status != XST_SUCCESS) {
#ifdef DEBUG
		//Get Timestamp
		unsigned long timestamp_ms = getElapsedRuntimeUS()/1000;

		myprintf("XIicPs_MasterSendPolled error at %d ms!\n\r", timestamp_ms);
#endif

		return XST_FAILURE;
	}

	/*
	 * Receive the data.
	 */
	Status = XIicPs_MasterRecvPolled(IicPs, Data, ByteCount, Address);
	if (Status != XST_SUCCESS) {
#ifdef DEBUG
		//Get Timestamp
		unsigned long timestamp_ms = getElapsedRuntimeUS()/1000;

		myprintf("XIicPs_MasterRecvPolled error at %d ms!\n\r", timestamp_ms);
#endif
		return XST_FAILURE;
	}

	/*
	 * Clear the IIC Repeated Start option.
	 */
	Status = XIicPs_ClearOptions(IicPs, XIICPS_REP_START_OPTION);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

//myprintf("[iic_read2] 0x%02X(0x%02X)=0x%X\n\r", Address, Register, *Data);

	return XST_SUCCESS;
}

///*
// * IIC Read X
// */
//static void iic_readx(XIicPs *IicPs, u8 Address, ZED_I2C_CONFIG Config[],
//		u32 Length) {
//	int i;
//
//	for (i = 0; i < Length; i++) {
//		iic_read2(IicPs, Address, Config[i].Reg, &Config[i].Data, 1);
//	}
//}
//
///*
// * IIC Read X 2
// */
//static void iic_readx2(XIicPs *IicPs, ZED_I2C_CONFIG2 Config[], u32 Length) {
//	int i;
//
//	for (i = 0; i < Length; i++) {
//		iic_read2(IicPs, Config[i].Addr, Config[i].Reg, &Config[i].Data, 1);
//	}
//}

/*
 * Read from Register
 */
int read_Reg(XIicPs *IicPs, u8 iic_addr, u8 reg) {
	u8 data;
	iic_read2(IicPs, iic_addr, reg, &data, 1);
	return data;
}