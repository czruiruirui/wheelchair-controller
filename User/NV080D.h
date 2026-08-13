#ifndef __NV080D_H__
#define __NV080D_H__

#include "main.h"
#include "gpio.h"
#include "tim.h"


extern void Delay_us(uint16_t us);
extern void Delay_ms(uint32_t ms);
void SendByte_OneLine(unsigned char SendByte);
void Start_OneLine();
void SendManyByte_OneLine(unsigned char *addr, unsigned char nums);




#endif
