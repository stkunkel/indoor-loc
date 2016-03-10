/*
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A 
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR 
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION 
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE 
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO 
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO 
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE 
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * 
 *
 * This file is a generated sample test application.
 *
 * This application is intended to test and/or illustrate some 
 * functionality of your system.  The contents of this file may
 * vary depending on the IP in your system and may use existing
 * IP driver functions.  These drivers will be generated in your
 * SDK application project when you run the "Generate Libraries" menu item.
 *
 */

#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "scugic_header.h"
#include "xgpio.h"
#include "gpio_header.h"
#include "xspi.h"
#include "spi_header.h"
#include "spi_intr_header.h"
#include "xtmrctr.h"
#include "tmrctr_header.h"
#include "tmrctr_intr_header.h"
#include "xdevcfg.h"
#include "devcfg_header.h"
#include "xdmaps.h"
#include "dmaps_header.h"
#include "xiicps.h"
#include "iicps_header.h"
#include "xqspips.h"
#include "qspips_header.h"
#include "xgpio.h"
#include "gpio_header.h"
#include "gpio_intr_header.h"
#include "xscutimer.h"
#include "scutimer_header.h"
#include "xscuwdt.h"
#include "scuwdt_header.h"
#include "xttcps.h"
#include "ttcps_header.h"

#include "xspi.h"
XSpi decaSpi;

#include "deca_device_api.h"
#include "deca_regs.h"
#include "deca_sleep.h"
#include "deca_spi.h"
#include "port.h"

/* Continuous wave duration, in milliseconds. */
#define CONT_WAVE_DURATION_MS 120000

/* Default communication configuration. Use channel 5 in this example as it is the recommended channel for crystal trimming operation. */
static dwt_config_t config = {
    5,               /* Channel number. */
    DWT_PRF_64M,     /* Pulse repetition frequency. */
    DWT_PLEN_1024,   /* Preamble length. */
    DWT_PAC32,       /* Preamble acquisition chunk size. Used in RX only. */
    9,               /* TX preamble code. Used in TX only. */
    9,               /* RX preamble code. Used in RX only. */
    1,               /* Use non-standard SFD (Boolean) */
    DWT_BR_110K,     /* Data rate. */
    DWT_PHRMODE_STD, /* PHY header mode. */
    (1025 + 64 - 32) /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

/* Recommended TX power and Pulse Generator delay values for the mode defined above. */
static dwt_txconfig_t txconfig = {
    0xC0,            /* PG delay. */
    0x25456585,      /* TX power. */
};

XGpio uwbGpio;

int ssTwrInit(void);
int ssTwrResp(void);
int txWait(void);
int rxWait(void);
int simpleTx(void);
int simpleRx(void);

void reset_DW1000(void)
{
    u32 uwbIn;
    XGpio_Initialize(&uwbGpio,XPAR_AXI_GPIO_UWB_DEVICE_ID);
    XGpio_DiscreteWrite(&uwbGpio,1,0); // reset on
    uwbIn = XGpio_DiscreteRead(&uwbGpio,2);
    /*
	printf("UBW reset active. Inputs 0x%x. Press any key\r\n", uwbIn);
	getchar();
	*/
    XGpio_DiscreteWrite(&uwbGpio,1,0xff); // reset off
    uwbIn = XGpio_DiscreteRead(&uwbGpio,2);
    while(0 == (uwbIn & 0x4)){
		//printf("UBW reset inactive. Inputs 0x%x\r\n", uwbIn);
	    uwbIn = XGpio_DiscreteRead(&uwbGpio,2);
    }
	printf("UBW reset completed\r\n");

    deca_sleep(2);
}

XGpio inGpio, outGpio;

// helpers for decawave debugging
int led = 0; // use led 4 for toggle
void ledToggle() {// toggle led
	led = XGpio_DiscreteRead(&outGpio,1);
	if (led & 0x8)
		XGpio_DiscreteWrite(&outGpio,1,led & 0x7);
	else
		XGpio_DiscreteWrite(&outGpio,1,led | 0x8);
};

int buttons() {
	return (0x0f & XGpio_DiscreteRead(&inGpio,2));
};

int main()
{
   Xil_ICacheEnable();
   Xil_DCacheEnable();
   
   
   print("---Entering main---\n\r");
   
   {
   	   printf("LEDs and switches\r\n");
   	    XGpio_Initialize(&inGpio,XPAR_AXI_GPIO_1_DEVICE_ID);
   	    XGpio_Initialize(&outGpio,XPAR_AXI_GPIO_0_DEVICE_ID);
   	    XGpio_DiscreteWrite(&outGpio,1,0); // leds off
      }

      {
   	   printf("Deca SPI test\r\n");

   	    if (0 != openspi()){
   	    	printf("Init SPI failed\r\n");
   	    } else {

   			// get switches
   			int sw = XGpio_DiscreteRead(&inGpio,1);
   			XGpio_DiscreteWrite(&outGpio,1,sw);
   			//printf("LED: %x\r\n",XGpio_DiscreteRead(&outGpio,1));

   			switch (sw & 0x7){

   			case 1:
   				printf("SS TWR INIT\r\n");
   				ssTwrInit();
   				break;
   			case 2:
   				printf("SS TWR RESP\r\n");
   				ssTwrResp();
   				break;
   			case 3:
   				printf("Simple TX\r\n");
   				simpleTx();
   				break;
   			case 4:
   				printf("Simple RX\r\n");
   				simpleRx();
   				break;
   			case 5:
   				printf("TX Wait\r\n");
   				txWait();
   				break;
   			case 6:
   				printf("RX Wait\r\n");
   				rxWait();
   				break;
   			default:

   			    /* Reset and initialise DW1000. */
   				reset_DW1000();
   				dwt_initialise(DWT_LOADNONE);
   				/* Configure DW1000. */
   				printf("UBW configuration sequence\r\n");
   				dwt_configure(&config);
   				dwt_configuretxrf(&txconfig);

   				/* Activate continuous wave mode. */
   				dwt_configcwmode(config.chan);

   				/* Wait for the wanted duration of the continuous wave transmission. */
   				printf("Waiting for UBW continuous wave transmission delay: %ds\r\n",CONT_WAVE_DURATION_MS/1000);
   				deca_sleep(CONT_WAVE_DURATION_MS);

   				/* Software reset of the DW1000 to deactivate continuous wave mode and go back to default state. Initialisation and configuration should be run
   				 * again if one wants to get the DW1000 back to normal operation. */
   				dwt_softreset();
   			}

   			printf("Deca test done. press any key\r\n");
   			getchar();
   	    }

      }


   print("---Exiting main---\n\r");
   Xil_DCacheDisable();
   Xil_ICacheDisable();
   return 0;
}
