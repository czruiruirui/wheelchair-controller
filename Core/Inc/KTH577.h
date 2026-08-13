#ifndef __KTH577_H__
#define __KTH577_H__


#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "switch.h"

// 协议定义
#define FRAME_HEADER        0xBB    // 帧头
#define CMD_READ_XY         0x00    // 读取 XY 数据命令
#define CMD_READ_POLAR      0x02    // 读取极坐标数据命令
#define STATUS_SUCCESS      0x00    // 成功状态
#define STATUS_FAIL         0x01    // 失败状态
#define RX_DATA_LENGTH      5       // 数据部分长度：状态(1) + 数据(4) + CRC(2)
#define DEAD_ZONE_THRESHOLD 500     // 死区阈值
#define CENTER_VALUE        2000    // 摇杆中心值

extern uint8_t Serial_RxPacket[];

//static volatile RxState rx_state = RX_STATE_WAIT_HEADER; // 接收状态
static volatile uint8_t rx_buffer[RX_DATA_LENGTH];       // 接收缓冲区
static volatile uint8_t rx_index = 0;                    // 接收索引
static volatile uint8_t current_cmd = 0;                 // 当前命令
static volatile uint8_t crc_buffer[2];                   // 接收的 CRC 值
extern uint8_t rx_byte; 																 // 接收字节缓存
extern uint16_t twos_complement_2,twos_complement_1;
extern uint16_t Joystick_X, Joystick_Y;
extern int16_t Coord_X, Coord_Y;
extern int16_t Joystick_PWM1, Joystick_PWM2;
extern uint8_t Serial_RxFlag;	


void KTH577_init();
uint16_t crc16_ccitt_false(const uint8_t *data, uint32_t length);
uint8_t yaogan_processing(uint8_t res[5]);



#endif
