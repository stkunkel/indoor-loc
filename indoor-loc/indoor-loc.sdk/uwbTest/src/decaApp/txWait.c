/*! ----------------------------------------------------------------------------
 *  @file    main.c
 *  @brief   TX then wait for response example code
 *
 *           This is a simple code example that sends a frame and then turns on the DW1000 receiver to receive a response. The response could be
 *           anything as no check is performed on it and it is only displayed in a local buffer but the sent frame is the one expected by the
 *           companion simple example "RX then send a response example code". After the response is received or if the reception timeouts, this code
 *           just go back to the sending of a new frame.
 *
 * @attention
 *
 * Copyright 2015 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#include "appCfg.h"

/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
static dwt_config_t config = {
    2,               /* Channel number. */
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


static uint8 tx_msg[] = {0xC5, 0, 'D', 'E', 'C', 'A', 'W', 'A', 'V', 'E', 0x43, 0x02, 0, 0};
/* Index to access to sequence number of the blink frame in the tx_msg array. */
#define BLINK_FRAME_SN_IDX 1

static uint8 rx_buffer[FRAME_LEN_MAX];

/* Hold copy of status register state here for reference, so reader can examine it at a breakpoint. */
static uint32 status_reg = 0;

/* Hold copy of frame length of frame received (if good), so reader can examine it at a breakpoint. */
static uint16 frame_len = 0;

/**
 * Application entry point.
 */
int txWait(void)
{
    /* Reset and initialise DW1000.
     * For initialisation, DW1000 clocks must be temporarily set to crystal speed. After initialisation SPI rate can be increased for optimum
     * performance. */
	int status;

    reset_DW1000(); /* Target specific drive of RSTn line into DW1000 low for a period. */
    //spi_set_rate_low();
    status = dwt_initialise(DWT_LOADNONE);
    if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);
    //spi_set_rate_high();

    /* Configure DW1000. See NOTE 5 below. */
    status = dwt_configure(&config);
    if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);

    /* Set delay to turn reception on after transmission of the frame. See NOTE 2 below. */
    dwt_setrxaftertxdelay(TX_TO_RX_DELAY_UUS);

    /* Set response frame timeout. */
    dwt_setrxtimeout(RX_RESP_TO_UUS);

    /* Loop forever sending and receiving frames periodically. */
    while (1)
    {
        /* Write frame data to DW1000 and prepare transmission. See NOTE 6 below.*/
    	status = dwt_writetxdata(sizeof(tx_msg), tx_msg, 0);
        if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);
    	status = dwt_writetxfctrl(sizeof(tx_msg), 0);
        if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);

        /* Start transmission, indicating that a response is expected so that reception is enabled immediately after the frame is sent. */
    	status = dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
        if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);

        /* We assume that the transmission is achieved normally, now poll for reception of a frame or error/timeout. See NOTE 7 below. */
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
        { printf("Waiting. status reg 0x%x\r\n",status_reg); };
        printf("Status reg now 0x%x\r\n",status_reg);

        if (status_reg & SYS_STATUS_RXFCG)
        {
            int i;

            printf("Frame received\r\n");
            /* Clear local RX buffer to avoid having leftovers from previous receptions. This is not necessary but is included here to aid reading
             * the RX buffer. */
            for (i = 0 ; i < FRAME_LEN_MAX; i++ )
            {
                rx_buffer[i] = 0;
            }

            /* A frame has been received, copy it to our local buffer. */
            frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
            printf("Frame length %d\r\n",frame_len);
            if (frame_len <= FRAME_LEN_MAX)
            {
            	dwt_readrxdata(rx_buffer, frame_len, 0);
            }
            for (i=0;i<frame_len;i++) {
            	printf("%x ",rx_buffer[i]);
            }
        	printf("\r\n");

            /* At this point, received frame can be examined in global "rx_buffer". An actual application would, for example, start by checking that
             * the format and/or data of the response are the expected ones. A developer might put a breakpoint here to examine this frame. */

            /* Clear good RX frame event in the DW1000 status register. */
            status = dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
            if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);
        }
        else
        {
            /* Clear RX error events in the DW1000 status register. */
        	status =  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
            if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);
        }

        /* Execute a delay between transmissions. */
        deca_sleep(TX_DELAY_MS);

        /* Increment the blink frame sequence number (modulo 256). */
        tx_msg[BLINK_FRAME_SN_IDX]++;
    }
}

/*****************************************************************************************************************************************************
 * NOTES:
 *
 * 1. The device ID is a hard coded constant in the blink to keep the example simple but for a real product every device should have a unique ID.
 *    For development purposes it is possible to generate a DW1000 unique ID by combining the Lot ID & Part Number values programmed into the
 *    DW1000 during its manufacture. However there is no guarantee this will not conflict with someone else�s implementation. We recommended that
 *    customers buy a block of addresses from the IEEE Registration Authority for their production items. See "EUI" in the DW1000 User Manual.
 * 2. TX to RX delay can be set to 0 to activate reception immediately after transmission. But, on the responder side, it takes time to process the
 *    received frame and generate the response (this has been measured experimentally to be around 70 �s). Using an RX to TX delay slightly less than
 *    this minimum turn-around time allows the application to make the communication efficient while reducing power consumption by adjusting the time
 *    spent with the receiver activated.
 * 3. This timeout is for complete reception of a frame, i.e. timeout duration must take into account the length of the expected frame. Here the value
 *    is arbitrary but chosen large enough to make sure that there is enough time to receive a complete frame sent by the "RX then send a response"
 *    example at the 110k data rate used (around 3 ms).
 * 4. In this example, maximum frame length is set to 127 bytes which is 802.15.4 UWB standard maximum frame length. DW1000 supports an extended frame
 *    length (up to 1023 bytes long) mode which is not used in this example.
 * 5. In a real application, for optimum performance within regulatory limits, it may be necessary to set TX pulse bandwidth and TX power, (using
 *    the dwt_configuretxrf API call) to per device calibrated values saved in the target system or the DW1000 OTP memory.
 * 6. dwt_writetxdata() takes the full size of tx_msg as a parameter but only copies (size - 2) bytes as the check-sum at the end of the frame is
 *    automatically appended by the DW1000. This means that our tx_msg could be two bytes shorter without losing any data (but the sizeof would not
 *    work anymore then as we would still have to indicate the full length of the frame to dwt_writetxdata()).
 * 7. We use polled mode of operation here to keep the example as simple as possible but all status events can be used to generate interrupts. Please
 *    refer to DW1000 User Manual for more details on "interrupts".
 * 8. The user is referred to DecaRanging ARM application (distributed with EVK1000 product) for additional practical example of usage, and to the
 *    DW1000 API Guide for more details on the DW1000 driver functions.
 ****************************************************************************************************************************************************/
