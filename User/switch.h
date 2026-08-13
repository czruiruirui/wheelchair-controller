#ifndef __SWITCH_H__
#define __SWITCH_H__

#include "main.h"
#include "NV080D.h"
#include "usart.h"
#include <stdio.h>


void switch_scan(void);

extern uint8_t dangwei;
extern uint8_t Btn_HORN, Btn_SOS, Btn_PLUS, Btn_REDUCE, Btn_POWER;
extern uint8_t Brake_State;
extern uint8_t Nav_Mode;
#endif


