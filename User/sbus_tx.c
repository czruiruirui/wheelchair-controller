#include "sbus_tx.h"
#include "KTH577.h"
#include "switch.h"

uint16_t Sbus_Ch[16];          /* debug watch: raw channel values */
static uint8_t sbus_frame[25]; /* packed SBUS frame */

/* CH9/SC 3-position switch value per gear: 1=TOP, 2=MIDDLE, 3=BOTTON */
static const uint16_t Gear_Ch9[3] = { SBUS_CH_N100, SBUS_CH_ZERO, SBUS_CH_P100 };

/* map normalized coord (-100..+100) back to SBUS raw (240..1024..1807) */
static uint16_t coord_to_sbus_raw(int16_t coord)
{
    int32_t raw = SBUS_CH_ZERO + ((int32_t)coord * (SBUS_CH_P100 - SBUS_CH_ZERO)) / 100;
    if (raw < SBUS_CH_N100) raw = SBUS_CH_N100;
    if (raw > SBUS_CH_P100) raw = SBUS_CH_P100;
    return (uint16_t)raw;
}

void SBUS_TX_Init(void)
{
    /* SBUS: 100000 baud, 8 data bits + even parity (=9B on F1), 2 stop bits */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 100000;
    huart2.Init.WordLength = UART_WORDLENGTH_9B;
    huart2.Init.StopBits = UART_STOPBITS_2;
    huart2.Init.Parity = UART_PARITY_EVEN;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
}

/* pack 16x 11-bit channels into 22 bytes, add header/flags/end, then send */
void SBUS_TX_SendFrame(void)
{
    uint16_t *ch = Sbus_Ch;

    /* ---- channel mapping (mirror the remote layout) ---- */
    ch[0]  = coord_to_sbus_raw(Coord_X);               /* CH1  right stick H (T1) */
    ch[1]  = coord_to_sbus_raw(-Coord_Y);              /* CH2  left stick V  (T3) mirror of Y, sign flipped */
    ch[2]  = coord_to_sbus_raw(-Coord_Y);              /* CH3  right stick V (T2), sign flipped */
    ch[3]  = coord_to_sbus_raw(Coord_X);               /* CH4  left stick H  (T4) mirror of X */
    ch[4]  = SBUS_CH_P100;                             /* CH5  knob V1 = +100 (unlock speed scale) */
    ch[5]  = SBUS_CH_P100;                             /* CH6  knob V2 = +100 (unlock speed scale) */
    ch[6]  = Nav_Mode ? SBUS_CH_P100 : SBUS_CH_N100; /* CH7  SA = NAV switch (SOS button): BOTTON=nav, TOP=manual */
    ch[7]  = (Nav_Mode || Brake_State) ? SBUS_CH_ZERO : SBUS_CH_N100; /* CH8 SB brake: TOP=braked only when manual+horn off, MIDDLE=released (nav or manual drive) */
    ch[8]  = Gear_Ch9[(dangwei >= 1 && dangwei <= 3) ? (dangwei - 1) : 0]; /* CH9 SC = speed gear: TOP=1档, MIDDLE=2档, BOTTON=3档 */
    ch[9]  = SBUS_CH_ZERO;                             /* CH10 SD = neutral */
    ch[10] = SBUS_CH_ZERO;
    ch[11] = SBUS_CH_ZERO;
    ch[12] = SBUS_CH_ZERO;
    ch[13] = SBUS_CH_ZERO;
    ch[14] = SBUS_CH_ZERO;
    ch[15] = SBUS_CH_ZERO;

    /* ---- pack 11-bit channels (standard SBUS packing) ---- */
    sbus_frame[0]  = 0x0F;
    sbus_frame[1]  = (uint8_t)(ch[0] & 0xFF);
    sbus_frame[2]  = (uint8_t)((ch[0] >> 8) | (ch[1] << 3));
    sbus_frame[3]  = (uint8_t)((ch[1] >> 5) | (ch[2] << 6));
    sbus_frame[4]  = (uint8_t)(ch[2] >> 2);
    sbus_frame[5]  = (uint8_t)((ch[2] >> 10) | (ch[3] << 1));
    sbus_frame[6]  = (uint8_t)((ch[3] >> 7) | (ch[4] << 4));
    sbus_frame[7]  = (uint8_t)((ch[4] >> 4) | (ch[5] << 7));
    sbus_frame[8]  = (uint8_t)(ch[5] >> 1);
    sbus_frame[9]  = (uint8_t)((ch[5] >> 9) | (ch[6] << 2));
    sbus_frame[10] = (uint8_t)((ch[6] >> 6) | (ch[7] << 5));
    sbus_frame[11] = (uint8_t)(ch[7] >> 3);
    sbus_frame[12] = (uint8_t)(ch[8] & 0xFF);
    sbus_frame[13] = (uint8_t)((ch[8] >> 8) | (ch[9] << 3));
    sbus_frame[14] = (uint8_t)((ch[9] >> 5) | (ch[10] << 6));
    sbus_frame[15] = (uint8_t)(ch[10] >> 2);
    sbus_frame[16] = (uint8_t)((ch[10] >> 10) | (ch[11] << 1));
    sbus_frame[17] = (uint8_t)((ch[11] >> 7) | (ch[12] << 4));
    sbus_frame[18] = (uint8_t)((ch[12] >> 4) | (ch[13] << 7));
    sbus_frame[19] = (uint8_t)(ch[13] >> 1);
    sbus_frame[20] = (uint8_t)((ch[13] >> 9) | (ch[14] << 2));
    sbus_frame[21] = (uint8_t)((ch[14] >> 6) | (ch[15] << 5));
    sbus_frame[22] = (uint8_t)(ch[15] >> 3);
    sbus_frame[23] = 0x00;   /* flags: ch17/ch18/frame-lost/failsafe all 0 */
    sbus_frame[24] = 0x00;   /* end byte */

    HAL_UART_Transmit(&huart2, sbus_frame, 25, 10);
}
