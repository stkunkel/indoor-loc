/*! ----------------------------------------------------------------------------
 * @file	deca_spi.c
 * @brief	SPI access functions
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */
#include <string.h>

#include "deca_spi.h"
#include "deca_sleep.h"
#include "../decadriver/deca_device_api.h"

#include "port.h"
extern XSpi decaSpi;

//#define SPIDBG

#ifdef SPIDBG
#define DPRINTF printf
#else
#define DPRINTF
#endif

#define SPIBUFS 1024

static u8 tbuf[SPIBUFS];
static u8 rbuf[SPIBUFS];

int writetospi_serial( uint16 headerLength,
			   	    const uint8 *headerBuffer,
					uint32 bodylength,
					const uint8 *bodyBuffer
				  );

int readfromspi_serial( uint16	headerLength,
			    	 const uint8 *headerBuffer,
					 uint32 readlength,
					 uint8 *readBuffer );
/*! ------------------------------------------------------------------------------------------------------------------
 * Function: openspi()
 *
 * Low level abstract function to open and initialise access to the SPI device.
 * returns 0 for success, or -1 for error
 */
int openspi(/*SPI_TypeDef* SPIx*/)
{
	// done by port.c, default SPI used is SPI1
	u32 status;
	status = XSpi_Initialize(&decaSpi, XPAR_AXI_QUAD_SPI_UWB_DEVICE_ID);

	XSpi_Reset(&decaSpi);

	// set master mode. manual SS is default
	// XSP_MANUAL_SSELECT_OPTION. might need to turn this off ...
	XSpi_SetOptions(&decaSpi, XSpi_GetOptions(&decaSpi) | XSP_MASTER_OPTION);

	XSpi_Enable(&decaSpi);

	XSpi_Start(&decaSpi);
	// polled mode: ints off
	XSpi_IntrGlobalDisable(&decaSpi);

	// dummy read to align SS with clock
	XSpi_SetSlaveSelect(&decaSpi,1);
	u8 buf = 0x5a;
    XSpi_Transfer(&decaSpi, &buf, &buf, 1);


    DPRINTF("%s:%d\r\n",__FUNCTION__,status);
	return status == XST_SUCCESS ? 0:-1;

} // end openspi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: closespi()
 *
 * Low level abstract function to close the the SPI device.
 * returns 0 for success, or -1 for error
 */
int closespi(void)
{
	//while (port_SPIx_busy_sending()); //wait for tx buffer to empty

	XSpi_Stop(&decaSpi);

	XSpi_Disable(&decaSpi);

	return 0;

} // end closespi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */
//#pragma GCC optimize ("O3")
int writetospi_serial
(
    uint16       headerLength,
    const uint8 *headerBuffer,
    uint32       bodylength,
    const uint8 *bodyBuffer
)
{

	int i=0;

	int status;

	DPRINTF("%s:hdrLen %d, bodyLen %d\r\n",__FUNCTION__,headerLength, bodylength);
	DPRINTF("Header: ");
	for (i=0;i<headerLength;i++)
		DPRINTF("0x%x ",headerBuffer[i]);
	DPRINTF("\r\nBody: ");
	for (i=0;i<bodylength;i++)
		DPRINTF("0x%x ",bodyBuffer[i]);
	DPRINTF("\r\n");


	// check length
	if (SPIBUFS < headerLength + bodylength){
		DPRINTF("%s:transfer size too large for buffer\r\n",__FUNCTION__);
		return -1;
	}
	// copy data to buffer
	for (i=0;i<headerLength;i++){
		tbuf[i] = headerBuffer[i];
	}
	for (i=0;i < bodylength;i++){
		tbuf[headerLength + i] = bodyBuffer[i];
	}

	XSpi_SetSlaveSelect(&decaSpi,1);
    status = XSpi_Transfer(&decaSpi, tbuf, 0, headerLength + bodylength);

    XSpi_SetSlaveSelect(&decaSpi,0);

    /*
    decaIrqStatus_t  stat ;

    stat = decamutexon() ;

    SPIx_CS_GPIO->BRR = SPIx_CS;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

    	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

    	SPIx->DR ;
    }

    for(i=0; i<bodylength; i++)
    {
     	SPIx->DR = bodyBuffer[i];

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

		SPIx->DR ;
	}

    SPIx_CS_GPIO->BSRR = SPIx_CS;

    decamutexoff(stat) ;
    */
    //DPRINTF("%s:%d\r\n",__FUNCTION__,status);

    return 0;
} // end writetospi()


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: readfromspi()
 *
 * Low level abstract function to read from the SPI
 * Takes two separate byte buffers for write header and read data
 * returns the offset into read buffer where first byte of read data may be found,
 * or returns -1 if there was an error
 */
//#pragma GCC optimize ("O3")
int readfromspi_serial
(
    uint16       headerLength,
    const uint8 *headerBuffer,
    uint32       readlength,
    uint8       *readBuffer
)
{
	int status;

	int i;

	DPRINTF("%s:hdrLen %d, readLen %d\r\n",__FUNCTION__,headerLength, readlength);
	DPRINTF("Header: ");
	for (i=0;i<headerLength;i++)
		DPRINTF("0x%x ",headerBuffer[i]);
	DPRINTF("\r\n");

	// check length
	if (SPIBUFS < headerLength + readlength){
		DPRINTF("%s:transfer size too large for buffer\r\n",__FUNCTION__);
		return -1;
	}
	// copy header data to buffer
	for (i=0;i<headerLength;i++){
		tbuf[i] = headerBuffer[i];
	}


	XSpi_SetSlaveSelect(&decaSpi,1);
    status = XSpi_Transfer(&decaSpi, tbuf, rbuf, headerLength + readlength);
    XSpi_SetSlaveSelect(&decaSpi,0);

	for (i=0;i < readlength;i++){
		readBuffer[i] = rbuf[headerLength + i];
	}

    DPRINTF("Read: ");
	for (i=0;i<readlength;i++)
		DPRINTF("0x%x ",readBuffer[i]);
	DPRINTF("\r\n");

    /*
	int i=0;
    decaIrqStatus_t  stat ;
    stat = decamutexon() ;

    // Wait for SPIx Tx buffer empty
    //while (port_SPIx_busy_sending());

    SPIx_CS_GPIO->BRR = SPIx_CS;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

     	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

     	readBuffer[0] = SPIx->DR ; // Dummy read as we write the header
    }

    for(i=0; i<readlength; i++)
    {
    	SPIx->DR = 0;  // Dummy write as we read the message body

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
 
	   	readBuffer[i] = SPIx->DR ;//port_SPIx_receive_data(); //this clears RXNE bit
    }

    SPIx_CS_GPIO->BSRR = SPIx_CS;

    decamutexoff(stat) ;
    */
	//DPRINTF("%s:%d\r\n",__FUNCTION__,status);

    return 0;
} // end readfromspi()
