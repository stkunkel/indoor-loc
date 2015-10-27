/*
 * Includes
 */
#include <stdio.h>
#include "xparameters.h"
#include "xiicps.h"
#include "iic_utils.h"

/*
 * Set IIC Clock
 */
int SetIiCSClk(volatile XIicPs *InstancePtr, u32 FsclHz) {
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
int iic_init(volatile XIicPs *IicPs, u16 DeviceId, u32 ClkRate) {
	int Status;
	XIicPs_Config *IicPs_Config;

	XIicPs_DisableAllInterrupts(IicPs->Config.BaseAddress);

	/*
	 * Initialize the IIC driver.
	 */
	IicPs_Config = XIicPs_LookupConfig(DeviceId);
	if (IicPs_Config == NULL) {
		//printf("No XIicPs instance found for ID %d\n\r", DeviceId);
		xil_printf("No XIicPs instance found for ID %d\n\r", DeviceId);
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(IicPs, IicPs_Config,
			IicPs_Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		//printf("XIicPs Initialization failed for ID %d\n\r", DeviceId);
		xil_printf("XIicPs Initialization failed for ID %d\n\r", DeviceId);
		return XST_FAILURE;
	}

	/*
	 * Set the IIC serial clock rate.
	 */
	//Status = XIicPs_SetSClk(IicPs, ClkRate);
	Status = SetIiCSClk(IicPs, ClkRate);
	if (Status != XST_SUCCESS) {
		//printf("Setting XIicPs clock rate failed for ID %d\n\r", DeviceId);
		xil_printf("Setting XIicPs clock rate failed for ID %d, Status: %d\n\r",
				DeviceId, Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
