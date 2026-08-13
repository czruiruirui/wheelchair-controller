#ifndef _TTL2CAN_H
#define _TTL2CAN_H

#include <stdio.h>
//#include "common.h" 
#include "main.h"
#include "string.h"

extern uint16_t s0,s1,s2,s3,s4,s5,s6,s7;
extern uint8_t transmit_data[8];
extern uint8_t transmit2_data[4];
extern uint8_t transmit4_data[8];
extern uint8_t Touch2PWM[5];
extern uint16_t twos_complement_2,twos_complement_1;
extern uint8_t transmit3_data[8];

extern uint8_t count;
extern uint16_t dirver,omg;
extern uint16_t Left_dirver,Right_dirver;


uint8_t TTL_data2can(uint8_t res[4]);
uint8_t yaogan_processing(uint8_t res[5]);
uint8_t free_ctrl_cmd(uint8_t res[4]);
uint8_t speed_processing(uint8_t res[4]);
uint8_t io_cmd(uint8_t mode);


#endif
















