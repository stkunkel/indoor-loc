/*! ----------------------------------------------------------------------------
 *  @file    main.c
 *  @brief   Single-sided two-way ranging (SS TWR) initiator example code
 *
 *           This is a simple code example which acts as the initiator in a SS TWR distance measurement exchange. This application sends a "poll"
 *           frame (recording the TX time-stamp of the poll), after which it waits for a "response" message from the "DS TWR responder" example
 *           code (companion to this application) to complete the exchange. The response message contains the remote responder's time-stamps of poll
 *           RX, and response TX. With this data and the local time-stamps, (of poll TX and response RX), this example application works out a value
 *           for the time-of-flight over-the-air and, thus, the estimated distance between the two devices, which it writes to the LCD.
 *
 * @attention
 *
 * Copyright 2015 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author Decawave
 */
#include <stdio.h>
#include <string.h>

#include "appCfg.h"

/* Default communication configuration. We use here EVK1000's mode 4. See NOTE 1 below. */
static dwt_config_t config = {
    2,               /* Channel number. */
    DWT_PRF_64M,     /* Pulse repetition frequency. */
    DWT_PLEN_128,    /* Preamble length. */
    DWT_PAC8,        /* Preamble acquisition chunk size. Used in RX only. */
    9,               /* TX preamble code. Used in TX only. */
    9,               /* RX preamble code. Used in RX only. */
    0,               /* Use non-standard SFD (Boolean) */
    DWT_BR_6M8,      /* Data rate. */
    DWT_PHRMODE_STD, /* PHY header mode. */
    (129 + 8 - 8)    /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};


/* Frames used in the ranging process. See NOTE 3 below. */
static uint8 tx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0xE0, 0, 0};

static uint8 rx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'V', 'E', 'W', 'A', 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
/* Length of the common part of the message (up to and including the function code, see NOTE 3 below). */
#define ALL_MSG_COMMON_LEN 10
/* Indexes to access some of the fields in the frames defined above. */
#define ALL_MSG_SN_IDX 2
#define RESP_MSG_POLL_RX_TS_IDX 10
#define RESP_MSG_RESP_TX_TS_IDX 14
#define RESP_MSG_TS_LEN 4
/* Frame sequence number, incremented after each transmission. */
static uint8 frame_seq_nb = 0;

/* Buffer to store received response message.
 * Its size is adjusted to longest frame that this example code is supposed to handle. */
#define RX_BUF_LEN 20
static uint8 rx_buffer[RX_BUF_LEN];

/* Hold copy of status register state here for reference, so reader can examine it at a breakpoint. */
static uint32 status_reg = 0;

/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
 * 1 uus = 512 / 499.2 �s and 1 �s = 499.2 * 128 dtu. */
#define UUS_TO_DWT_TIME 65536

/* Delay between frames, in UWB microseconds. See NOTE 1 below. */
#define POLL_TX_TO_RESP_RX_DLY_UUS 140

/* Receive response timeout. See NOTE 5 below. */
// #define RESP_RX_TIMEOUT_UUS 910 // was 210 AKU
#define RESP_RX_TIMEOUT_UUS 910 // default

/* Speed of light in air, in metres per second. */
#define SPEED_OF_LIGHT 299702547

/* Hold copies of computed time of flight and distance here for reference, so reader can examine it at a breakpoint. */
static double tof;
static double distance;

/* String used to display measured distance on LCD screen (16 characters maximum). */
char dist_str[16] = {0};

/* Declaration of static functions. */
static void resp_msg_get_ts(uint8 *ts_field, uint32 *ts);

static u32 tx_delay;
static u32 rx_delay;
static int btn = 0;

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn main()
 *
 * @brief Application entry point.
 *
 * @param  none
 *
 * @return none
 */
int ssTwrInit(void)
{

    /* Reset and initialise DW1000.
     * For initialisation, DW1000 clocks must be temporarily set to crystal speed. After initialisation SPI rate can be increased for optimum
     * performance. */
	int status;

	reset_DW1000(); /* Target specific drive of RSTn line into DW1000 low for a period. */
    //spi_set_rate_low();
    status = dwt_initialise(DWT_LOADUCODE);
    if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);
    //spi_set_rate_high();

    /* Configure DW1000. See NOTE 6 below. */
    status = dwt_configure(&config);
    if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);

    // read otp
    uint32_t otpVal[0x20];
    dwt_otpread(0,otpVal,0x20);
    printf("OTP   6: 0x%x\r\n",otpVal[6]);
    printf("OTP   7: 0x%x\r\n",otpVal[7]);
    printf("OTP x16: 0x%x\r\n",otpVal[0x16]);
    printf("OTP x17: 0x%x\r\n",otpVal[0x17]);

    /* Apply default antenna delay value. See NOTE 2 below. */
    printf("antenna delays: default TX: %d, default RX: %d, evk 16m: %d, evk 64m: %d\r\n",TX_ANT_DLY,RX_ANT_DLY,DWT_RF_DELAY_16M,DWT_RF_DELAY_64M);
    tx_delay = TX_ANT_DLY;
    rx_delay = RX_ANT_DLY;
    dwt_setrxantennadelay(rx_delay);
    dwt_settxantennadelay(tx_delay);

    /* Set expected response's delay and timeout. See NOTE 1 and 5 below.
     * As this example only handles one incoming frame with always the same delay and timeout, those values can be set here once for all. */
    dwt_setrxaftertxdelay(POLL_TX_TO_RESP_RX_DLY_UUS);
    dwt_setrxtimeout(RESP_RX_TIMEOUT_UUS);

    btn = buttons();

    printf("%s entering main loop\r\n",__FUNCTION__);

    /* Loop forever initiating ranging exchanges. */
    while (1)
    {
        /* Write frame data to DW1000 and prepare transmission. See NOTE 7 below. */
        tx_poll_msg[ALL_MSG_SN_IDX] = frame_seq_nb;
        status = dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
        if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);
        status = dwt_writetxdata(sizeof(tx_poll_msg), tx_poll_msg, 0);
        if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);
        status = dwt_writetxfctrl(sizeof(tx_poll_msg), 0);
        if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);

        /* Start transmission, indicating that a response is expected so that reception is enabled automatically after the frame is sent and the delay
         * set by dwt_setrxaftertxdelay() has elapsed. */
        status = dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
        if (DWT_SUCCESS != status) printf("API error line %d\r\n",__LINE__);

        /* We assume that the transmission is achieved correctly, poll for reception of a frame or error/timeout. See NOTE 8 below. */
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
        { } ; // printf("Waiting. status reg 0x%x\r\n",status_reg); };
        //printf("Status reg now 0x%x\r\n",status_reg);

        if (SYS_STATUS_RXRFTO & status_reg)
            printf("RX timeout\r\n");

        /* Increment frame sequence number after transmission of the poll message (modulo 256). */
        frame_seq_nb++;

        if (status_reg & SYS_STATUS_RXFCG)
        {
            uint32 frame_len;

            //printf("Check RX\r\n");
            /* Clear good RX frame event in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

            /* A frame has been received, read it into the local buffer. */
            frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
            if (frame_len <= RX_BUF_LEN)
            {
                dwt_readrxdata(rx_buffer, frame_len, 0);
            }

            /* Check that the frame is the expected response from the companion "SS TWR responder" example.
             * As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame. */
            rx_buffer[ALL_MSG_SN_IDX] = 0;
            if (memcmp(rx_buffer, rx_resp_msg, ALL_MSG_COMMON_LEN) == 0)
            {
                uint32 poll_tx_ts, resp_rx_ts, poll_rx_ts, resp_tx_ts;
                int32 rtd_init, rtd_resp;

                /* Retrieve poll transmission and response reception timestamps. See NOTE 9 below. */
                poll_tx_ts = dwt_readtxtimestamplo32();
                resp_rx_ts = dwt_readrxtimestamplo32();

                /* Get timestamps embedded in response message. */
                resp_msg_get_ts(&rx_buffer[RESP_MSG_POLL_RX_TS_IDX], &poll_rx_ts);
                resp_msg_get_ts(&rx_buffer[RESP_MSG_RESP_TX_TS_IDX], &resp_tx_ts);

                /* Compute time of flight and distance. */
                rtd_init = resp_rx_ts - poll_tx_ts;
                rtd_resp = resp_tx_ts - poll_rx_ts;

                tof = ((rtd_init - rtd_resp) / 2.0) * DWT_TIME_UNITS;
                distance = tof * SPEED_OF_LIGHT;

                /* Display computed distance on LCD. */
                //sprintf(dist_str, "DIST: %3.2f m", distance);
                sprintf(dist_str, "%3.2f", distance);
                printf("%s\r\n",dist_str);
                //lcd_display_str(dist_str);

            }
        }
        else
        {
            /* Clear RX error events in the DW1000 status register. */
            printf("Errors occured. Clearing up\r\n");
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
        }

        /* Execute a delay between ranging exchanges. */
        //printf("Delay %dms\r\n",RNG_DELAY_MS);
        deca_sleep(RNG_DELAY_MS);

        // toggle led
		ledToggle();
		// update antenna
		if (buttons() & 1) {
		    tx_delay -= 10; // >>= 1;
		    rx_delay -= 10; //>>= 1;
		    dwt_setrxantennadelay(rx_delay);
		    dwt_settxantennadelay(tx_delay);
		    printf("Decreased antenna delay to 0x%x\r\n",tx_delay);
		}
		if (buttons() & 2) {
			tx_delay += 10;
			rx_delay += 10;
		    dwt_setrxantennadelay(rx_delay);
		    dwt_settxantennadelay(tx_delay);
		    printf("Increased antenna delay to 0x%x\r\n",tx_delay);
		}

    }
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn resp_msg_get_ts()
 *
 * @brief Read a given timestamp value from the response message. In the timestamp fields of the response message, the
 *        least significant byte is at the lower address.
 *
 * @param  ts_field  pointer on the first byte of the timestamp field to get
 *         ts  timestamp value
 *
 * @return none
 */
static void resp_msg_get_ts(uint8 *ts_field, uint32 *ts)
{
    int i;
    *ts = 0;
    for (i = 0; i < RESP_MSG_TS_LEN; i++)
    {
        *ts += ts_field[i] << (i * 8);
    }
}

/*****************************************************************************************************************************************************
 * NOTES:
 *
 * 1. The single-sided two-way ranging scheme implemented here has to be considered carefully as the accuracy of the distance measured is highly
 *    sensitive to the clock offset error between the devices and the length of the response delay between frames. To achieve the best possible
 *    accuracy, this response delay must be kept as low as possible. In order to do so, 6.8 Mbps data rate is used in this example and the response
 *    delay between frames is defined as low as possible. The user is referred to User Manual for more details about the single-sided two-way ranging
 *    process.
 * 2. The sum of the values is the TX to RX antenna delay, experimentally determined by a calibration process. Here we use a hard coded typical value
 *    but, in a real application, each device should have its own antenna delay properly calibrated to get the best possible precision when performing
 *    range measurements.
 * 3. The frames used here are Decawave specific ranging frames, complying with the IEEE 802.15.4 standard data frame encoding. The frames are the
 *    following:
 *     - a poll message sent by the initiator to trigger the ranging exchange.
 *     - a response message sent by the responder to complete the exchange and provide all information needed by the initiator to compute the
 *       time-of-flight (distance) estimate.
 *    The first 10 bytes of those frame are common and are composed of the following fields:
 *     - byte 0/1: frame control (0x8841 to indicate a data frame using 16-bit addressing).
 *     - byte 2: sequence number, incremented for each new frame.
 *     - byte 3/4: PAN ID (0xDECA).
 *     - byte 5/6: destination address, see NOTE 4 below.
 *     - byte 7/8: source address, see NOTE 4 below.
 *     - byte 9: function code (specific values to indicate which message it is in the ranging process).
 *    The remaining bytes are specific to each message as follows:
 *    Poll message:
 *     - no more data
 *    Response message:
 *     - byte 10 -> 13: poll message reception timestamp.
 *     - byte 14 -> 17: response message transmission timestamp.
 *    All messages end with a 2-byte checksum automatically set by DW1000.
 * 4. Source and destination addresses are hard coded constants in this example to keep it simple but for a real product every device should have a
 *    unique ID. Here, 16-bit addressing is used to keep the messages as short as possible but, in an actual application, this should be done only
 *    after an exchange of specific messages used to define those short addresses for each device participating to the ranging exchange.
 * 5. This timeout is for complete reception of a frame, i.e. timeout duration must take into account the length of the expected frame. Here the value
 *    is arbitrary but chosen large enough to make sure that there is enough time to receive the complete response frame sent by the responder at the
 *    6.8M data rate used (around 200 �s).
 * 6. In a real application, for optimum performance within regulatory limits, it may be necessary to set TX pulse bandwidth and TX power, (using
 *    the dwt_configuretxrf API call) to per device calibrated values saved in the target system or the DW1000 OTP memory.
 * 7. dwt_writetxdata() takes the full size of the message as a parameter but only copies (size - 2) bytes as the check-sum at the end of the frame is
 *    automatically appended by the DW1000. This means that our variable could be two bytes shorter without losing any data (but the sizeof would not
 *    work anymore then as we would still have to indicate the full length of the frame to dwt_writetxdata()).
 * 8. We use polled mode of operation here to keep the example as simple as possible but all status events can be used to generate interrupts. Please
 *    refer to DW1000 User Manual for more details on "interrupts". It is also to be noted that STATUS register is 5 bytes long but, as the event we
 *    use are all in the first bytes of the register, we can use the simple dwt_read32bitreg() API call to access it instead of reading the whole 5
 *    bytes.
 * 9. The high order byte of each 40-bit time-stamps is discarded here. This is acceptable as, on each device, those time-stamps are not separated by
 *    more than 2**32 device time units (which is around 67 ms) which means that the calculation of the round-trip delays can be handled by a 32-bit
 *    subtraction.
 * 10. The user is referred to DecaRanging ARM application (distributed with EVK1000 product) for additional practical example of usage, and to the
 *     DW1000 API Guide for more details on the DW1000 driver functions.
 ****************************************************************************************************************************************************/
