#include "KTH577.h"
#include <stdlib.h>

uint8_t rx_byte; // 接收字节缓存
uint8_t RxState = 0;
uint8_t Serial_RxPacket[60];	
uint16_t twos_complement_2,twos_complement_1;
uint8_t Touch2PWM[5];
uint16_t Joystick_X = 0, Joystick_Y = 0;      /* joystick raw X/Y coordinate from packet */
int16_t Coord_X = 0, Coord_Y = 0;             /* SBUS-style normalized coord: -100 .. +100 */
#define JOY_DEADZONE_PCT  15   //dead zone
/* 3-gear speed limit: gear1=40%, gear2=70%, gear3=100% of full stick output */
static const int16_t Gear_Scale[3] = {40, 70, 100}; //speed level
int16_t Joystick_PWM1 = 0, Joystick_PWM2 = 0; /* signed PWM output per channel */
uint8_t Serial_RxFlag;	
extern uint8_t rx_flag;

void KTH577_init()
{
	//HAL_GPIO_WritePin(SENSOR_PORT,SENSOR_PIN,GPIO_PIN_SET); // TODO: sensor power pin TBD
	MX_USART3_UART_Init();
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	static volatile uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
		if (RxState == 0)
		{
			if (rx_byte == 0xBB)
			{
				RxState = 1;
			}
		}
		else if (RxState == 1)
		{
			if (rx_byte == 0X00)
			{
				RxState = 2;
				pRxPacket = 0;
			}
		
		}
		else if (RxState == 2)
		{
			if (rx_byte == 0x00)
			{
				RxState = 3;
				pRxPacket = 0;
			}
		
		}
		else if (RxState == 3)
		{
			Serial_RxPacket[pRxPacket] = rx_byte;
			pRxPacket ++;
			if (pRxPacket >= 5)
			{
				RxState = 6;
			}
		}
		else if (RxState == 6)
		{
				RxState = 0;         //重置标志位
				//speed_processing(Serial_RxPacket);
				yaogan_processing(Serial_RxPacket);
				//TTL_data2can(Touch2PWM);		
				//TTL_data2can(transmit2_data);
				//uart3SendChars(buf,12);
				//free_ctrl_cmd(Serial_RxPacket);
				//io_cmd(i);
				//count ++;
				Serial_RxFlag = 1;   //接受标志位
		}
		//USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		HAL_UART_Receive_IT(&huart3, (uint8_t *)&rx_byte, 1);

}


uint16_t little_endian_to_uint16(uint8_t low_byte, uint8_t high_byte) {
    return (high_byte << 8) | low_byte;
}

int16_t calculate_pwm(int16_t touch_point_hex, int16_t center_hex, int16_t outer_range, int16_t pwm_range) {
    // ?????
    int16_t offset = center_hex - touch_point_hex;
    // ?????
    float normalized_offset = (float)offset / outer_range;
    // ??PWM?
    int16_t pwm = (int16_t)(normalized_offset * pwm_range);

    return pwm;
}

// 将PWM值转成补码值
uint16_t pwm_to_twos_complement(int16_t pwm_value, uint8_t bits) {
    // PWM
    if (pwm_value < 0) {
        pwm_value = (1 << bits) + pwm_value;
    }
    // bits
    return pwm_value & ((1 << bits) - 1);
}

void twos_complement_to_little_endian(uint16_t twos_complement, uint8_t *low_byte, uint8_t *high_byte) {
    *low_byte = twos_complement & 0xFF;  // 
    *high_byte = (twos_complement >> 8) & 0xFF; // 
}


uint16_t dead_zone_filter(uint16_t raw_value) {
    if (abs(raw_value - CENTER_VALUE) < DEAD_ZONE_THRESHOLD) {
        return CENTER_VALUE;
    } else {
        return raw_value;
    }
}

//输出两路PWM值
uint8_t yaogan_processing(uint8_t res[5])
{

    int16_t center_hex = 0x07D0;                  // 0x07D0 = 2000
    int16_t outer_range = 2500;                   // (4500 - 500 = 4000,2000)
    int16_t pwm_range = 500+(1000*dangwei/5);                      // PWM:-500 到 +500
    uint8_t bits = 10;                            // 

    uint16_t touch_point_hex_1 = little_endian_to_uint16(res[1], res[0]);
    uint16_t touch_point_hex_2 = little_endian_to_uint16(res[3], res[2]);

    Joystick_X = touch_point_hex_1;   /* X coordinate (big-endian 16bit) */
    Joystick_Y = touch_point_hex_2;   /* Y coordinate (big-endian 16bit) */

    /* SBUS-style mapping: same semantics as fs_sbus_process_msg
       raw center(0x07C0/0x081E) -> 0, +-outer_range(2500) -> -+100
       deadzone +-2 percent, clamp to [-100, +100] */
    {
        int32_t nx = ((int32_t)0x07C0 - (int32_t)touch_point_hex_1) * 100 / outer_range;
        int32_t ny = ((int32_t)0x081E - (int32_t)touch_point_hex_2) * 100 / outer_range;
        if (nx > -JOY_DEADZONE_PCT && nx < JOY_DEADZONE_PCT) nx = 0;
        if (ny > -JOY_DEADZONE_PCT && ny < JOY_DEADZONE_PCT) ny = 0;
        if (nx > 100)  nx = 100;
        if (nx < -100) nx = -100;
        if (ny > 100)  ny = 100;
        if (ny < -100) ny = -100;
        /* apply gear speed limit (dangwei: 1..3) */
        {
            uint8_t g = (dangwei >= 1 && dangwei <= 3) ? (dangwei - 1) : 0;
            nx = nx * Gear_Scale[g] / 100;
            ny = ny * Gear_Scale[g] / 100;
        }
        Coord_X = (int16_t)nx;
        Coord_Y = (int16_t)ny;
    }

    // ?????PWM1
    int16_t pwm_value_1 = calculate_pwm(touch_point_hex_1, 0x07C0, outer_range, pwm_range);
    twos_complement_1 = pwm_to_twos_complement(pwm_value_1, bits);

    // ?????PWM2
    int16_t pwm_value_2 = calculate_pwm(touch_point_hex_2, 0x081E, outer_range, pwm_range);
    Joystick_PWM1 = pwm_value_1;
    Joystick_PWM2 = pwm_value_2;
    twos_complement_2 = pwm_to_twos_complement(pwm_value_2, bits);
	
    twos_complement_to_little_endian(twos_complement_1, &Touch2PWM[1], &Touch2PWM[0]);
    twos_complement_to_little_endian(twos_complement_2, &Touch2PWM[3], &Touch2PWM[2]);
	
		return 0; /* result via global Touch2PWM[0..3] */

}




















