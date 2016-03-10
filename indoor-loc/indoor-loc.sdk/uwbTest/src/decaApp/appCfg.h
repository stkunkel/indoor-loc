// general defines for all example apps

#include "deca_device_api.h"
#include "deca_regs.h"
#include "deca_sleep.h"
#include "port.h"
#include <stdio.h>

// don't define config globally - twr and non twr use diferent modes

/* Index to access to sequence number of the blink frame in the tx_msg array. */
#define BLINK_FRAME_SN_IDX 1

/* Inter-frame delay period, in milliseconds. */
#define TX_DELAY_MS 1000

/* Delay from end of transmission to activation of reception, expressed in UWB microseconds (1 uus is 512/499.2 microseconds). See NOTE 2 below. */
#define TX_TO_RX_DELAY_UUS 60

/* Receive response timeout, expressed in UWB microseconds. See NOTE 3 below. */
#define RX_RESP_TO_UUS 5000

/* Buffer to store received frame. See NOTE 4 below. */
#define FRAME_LEN_MAX 127

// ----------------------
//from older evk driver
//these are default antenna delays for EVB1000, these can be used if there is no calibration data in the DW1000,
//or instead of the calibration data
#define DWT_PRF_64M_RFDLY   (514.462f)
#define DWT_PRF_16M_RFDLY   (513.9067f)

#define DWT_RF_DELAY_16M ((uint16) ((DWT_PRF_16M_RFDLY/ 2.0) * 1e-9 / DWT_TIME_UNITS))
#define DWT_RF_DELAY_64M ((uint16) ((DWT_PRF_64M_RFDLY/ 2.0) * 1e-9 / DWT_TIME_UNITS))
// ----------------------

/*
7.1 Antenna Delay
The antenna delay may need changing if a different antenna is being used.
Note: If the antenna delay value is set too large, it results in negative RTD calculation results (internally to
the software) and these RTD values are discarded as bad and no RTD / distance measurement will be
reported. In using the system, if the communication seems to be working, but the time-of-flight status lines
are not updating, then this may be because the antenna delay is set to too large a value. This can be
checked by clearing the antenna delays to zero. To tune the antenna delay to the correct value is a process
of trial and error, tweaking the antenna delay until the average distance reported matches the real antenna-
to-antenna distance measured with a tape measure.
 */

// current driver version.
/* Default antenna delay values for 64 MHz PRF. See NOTE 2 below. */
#define DRV3_DEFAULT_TX_ANT_DLY 16436
#define DRV3_DEFAULT_RX_ANT_DLY 16436
// according to user guide, these delays might be wrong!
//#define USE_DEF_DELAYS
#ifdef USE_DEF_DELAYS
#define TX_ANT_DLY DRV3_DEFAULT_TX_ANT_DLY
#define RX_ANT_DLY DRV3_DEFAULT_RX_ANT_DLY
#else
#define TX_ANT_DLY 16540
#define RX_ANT_DLY 16540
#endif

// override delays
//#define ZERO_DELAY
#ifdef ZERO_DELAY
#undef TX_ANT_DLY
#undef RX_ANT_DLY
#define TX_ANT_DLY 0
#define RX_ANT_DLY 0
#endif

/* Inter-ranging delay period, in milliseconds. */
#define RNG_DELAY_MS 50 // 1000


#include "xgpio.h"
extern XGpio outGpio;
extern XGpio inGpio;
extern int led; // use led 4 for toggle

void ledToggle();
int buttons();
