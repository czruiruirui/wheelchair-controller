#ifndef __SBUS_TX_H__
#define __SBUS_TX_H__

#include "main.h"
#include "usart.h"

/* SBUS frame: 25 bytes, 100000 baud 8E2, frame every ~10ms
   Channel raw values (same as remote):
   N100 = 240 (0x00F0), 0 = 1024 (0x0400), P100 = 1807 (0x070F) */
#define SBUS_CH_N100   240
#define SBUS_CH_ZERO   1024
#define SBUS_CH_P100   1807

extern uint16_t Sbus_Ch[16];   /* 16 channels raw value, for debug watch */

void SBUS_TX_Init(void);
void SBUS_TX_SendFrame(void);

#endif
