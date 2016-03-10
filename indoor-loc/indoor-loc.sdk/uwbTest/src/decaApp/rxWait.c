/*! ----------------------------------------------------------------------------
 *  @file    main.c
 *  @brief   RX then send a response example code
 *
 *           This is a simple code example that turns on the DW1000 receiver to receive a frame, (expecting the frame as sent by the companion simple
 *           example "TX then wait for response example code"). When a frame is received and validated as the expected frame a response message is
 *           sent, after which the code returns to await reception of another frame.
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

static uint8 tx_msg[] = {0x41, 0x8C, 0, 0x9A, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 'D', 'W', 0x10, 0x00, 0, 0, 0, 0};
/* Indexes to access to sequence number and destination address of the data frame in the tx_msg array. */
#define DATA_FRAME_SN_IDX 2
#define DATA_FRAME_DEST_IDX 5

static uint8 rx_buffer[FRAME_LEN_MAX];
/* Index to access to source address of the blink frame in the rx_buffer array. */
#define BLINK_FRAME_SRC_IDX 2

/* Hold copy of status register state here for reference, so reader can examine it at a breakpoint. */
static uint32 status_reg = 0;

/* Hold copy of frame length of frame received (if good), so reader can examine it at a breakpoint. */
static uint16 frame_len = 0;

/**
 * Application entry point.
 */
int rxWait(void)
{
    /* Reset and initialise DW1000.
     * For initialisation, DW1000 clocks must be temporarily set to crystal speed. After initialisation SPI rate can be increased for optimum
     * performance. */
	int i;

    reset_DW1000(); /* Target specific drive of RSTn line into DW1000 low for a period. */
    //spi_set_rate_low();
    dwt_initialise(DWT_LOADNONE);
    //spi_set_rate_high();

    /* Configure DW1000. See NOTE 2 below. */
    dwt_configure(&config);

    /* Loop forever sending and receiving frames periodically. */
    while (1)
    {
        /* Activate reception immediately. See NOTE 3 below. */
        dwt_rxenable(0);

        /* Poll until a frame is properly received or an error occurs. See NOTE 4 below.
         * STATUS register is 5 bytes long but, as the events we are looking at are in the lower bytes of the register, we can use this simplest API
         * function to access it. */
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
        { };
        printf("Status reg now 0x%x\r\n",status_reg);

        if (status_reg & SYS_STATUS_RXFCG)
        {
            /* A frame has been received, read it into the local buffer. */
            frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
            if (frame_len <= FRAME_LEN_MAX)
            {
                dwt_readrxdata(rx_buffer, frame_len, 0);
            }

            /* Clear good RX frame event in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

            for (i=0;i<frame_len;i++) {
            	printf("%x ",rx_buffer[i]);
            }
            printf("\r\n");

            /* Validate the frame is the one expected as sent by "TX then wait for a response" example. */
            if ((frame_len == 14) && (rx_buffer[0] == 0xC5) && (rx_buffer[10] == 0x43) && (rx_buffer[11] == 0x2))
            {
                int i;

                /* Copy source address of blink in response destination address. */
                for (i = 0; i < 8; i++)
                {
                    tx_msg[DATA_FRAME_DEST_IDX + i] = rx_buffer[BLINK_FRAME_SRC_IDX + i];
                }

                /* Write response frame data to DW1000 and prepare transmission. See NOTE 5 below.*/
                dwt_writetxdata(sizeof(tx_msg), tx_msg, 0);
                dwt_writetxfctrl(sizeof(tx_msg), 0);

                /* Send the response. */
                dwt_starttx(DWT_START_TX_IMMEDIATE);

                /* Poll DW1000 until TX frame sent event set. */
                while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
                { };

                /* Clear TX frame sent event. */
                dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

                /* Increment the data frame sequence number (modulo 256). */
                tx_msg[DATA_FRAME_SN_IDX]++;
            }
        }
        else
        {
            /* Clear RX error events in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
            printf("Some RX errors ...\r\n");
        }
    }
}

/*****************************************************************************************************************************************************
 * NOTES:
 *
 * 1. In this example, maximum frame length is set to 127 bytes which is 802.15.4 UWB standard maximum frame length. DW1000 supports an extended frame
 *    length (up to 1023 bytes long) mode which is not used in this example.
 * 2. In a real application, for optimum performance within regulatory limits, it may be necessary to set TX pulse bandwidth and TX power, (using
 *    the dwt_configuretxrf API call) to per device calibrated values saved in the target system or the DW1000 OTP memory.
 * 3. Manual reception activation is performed here but DW1000 offers several features that can be used to handle more complex scenarios or to
 *    optimise system's overall performance (e.g. timeout after a given time, automatic re-enabling of reception in case of errors, etc.).
 * 4. We use polled mode of operation here to keep the example as simple as possible but all status events can be used to generate interrupts. Please
 *    refer to DW1000 User Manual for more details on "interrupts".
 * 5. dwt_writetxdata() takes the full size of tx_msg as a parameter but only copies (size - 2) bytes as the check-sum at the end of the frame is
 *    automatically appended by the DW1000. This means that our tx_msg could be two bytes shorter without losing any data (but the sizeof would not
 *    work anymore then as we would still have to indicate the full length of the frame to dwt_writetxdata()).
 * 6. The user is referred to DecaRanging ARM application (distributed with EVK1000 product) for additional practical example of usage, and to the
 *    DW1000 API Guide for more details on the DW1000 driver functions.
 ****************************************************************************************************************************************************/
