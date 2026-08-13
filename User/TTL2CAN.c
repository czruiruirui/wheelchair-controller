#include "TTL2CAN.h"

uint8_t transmit_data[8];
uint8_t transmit3_data[8];
uint8_t transmit2_data[4];
uint8_t transmit4_data[8];
uint8_t transmit5_data[8];
uint8_t AliveCounter[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
uint16_t Left_dirver=0,Right_dirver=0;
uint16_t driver=0,omg=0;
uint8_t count=0;


/*线速度、角速度处理*/
uint8_t speed_processing(uint8_t res[4]){

	memset(transmit2_data,0,sizeof(transmit2_data));// 清空速度处理数据的数组
	Left_dirver  = (res[1] << 8) | res[0];
	Right_dirver = (res[3] << 8) | res[2];
	driver = ((Left_dirver + Right_dirver) / 2);
	omg = ((Left_dirver - Right_dirver) *100 / 55);
	transmit2_data[0] = driver & 0xff;  		 // 线速度的低位
	transmit2_data[1] = driver >> 8;		 	 // 线速度的高位
	transmit2_data[2] = omg & 0x00ff;	 		 // 角速度的低位
	transmit2_data[3] = omg >> 8;  				 // 角速度的高位
												 //
	return 0; /* result via global transmit2_data */

}
/*
	运动控制指令-控制帧:ctrl_cmd
	ID:0x18C4D1D0
	处理运动控制
*/
uint8_t TTL_data2can(uint8_t res[4])
{

	transmit_data[6]=0x00;
	memset(transmit_data,0,sizeof(transmit_data));					// 清空存放CAN数据的数组
    transmit_data[0] = (res[0] & 0x0F) << 4 | 0x03;
    transmit_data[1] = (res[1] & 0x0F) << 4 | (res[0] & 0xF0) >> 4;
    transmit_data[2] = ((res[1] >> 4) & 0x0f) | (res[2] & 0x0F) << 4;
	transmit_data[3] = (res[3] & 0x0F) << 4 | (res[2] & 0xF0) >> 4;		//
	transmit_data[4] = (res[3] >> 4) & 0x0f ;
	transmit_data[5] = 0;
	transmit_data[6] = AliveCounter[count]<<4;
	count++; /* alive counter must roll 0..15 */
	if (count == 16)
	{
		count =0;
	}
	transmit_data[7] = transmit_data[0] ^ transmit_data[1] ^ transmit_data[2] ^ transmit_data[3]
						^transmit_data[4] ^ transmit_data[5] ^ transmit_data[6] ^ transmit_data[7];
	return 0; /* result via global transmit_data */
}

/*
	自动控制指令：free_ctrl_cmd
	ID：0x18C4D2D0
	处理自由控制的运动
*/
/**
  * 自动控制指令:free_ctrl_cmd
  * I          D:0x18C4D2D0
  * 作        用:生成发送CAN协议处理自由控制的数据域
  */
uint8_t free_ctrl_cmd(uint8_t res[4])
{

	transmit3_data[6]=0x00;
	memset(transmit3_data,0,sizeof(transmit3_data));					// 清空存放CAN数据的数组
    transmit3_data[0] = (res[2] & 0x0F) << 4 | 0x04;                
    transmit3_data[1] = (res[3] & 0x0F) << 4 | (res[2] & 0xF0) >> 4;
    transmit3_data[2] = ((res[3] >> 4) & 0x0f) | (res[0] & 0x0F) << 4;
	transmit3_data[3] = (res[1] & 0x0F) << 4 | (res[0] & 0xF0) >> 4;		//
	transmit3_data[4] = (res[1] >> 4) & 0x0f ;                      
	transmit3_data[5] = 0;                                          
	transmit3_data[6] = AliveCounter[count]<<4;
	count++; /* alive counter must roll 0..15 */                     
	if (count == 16)
	{
		count =0;
	}
	transmit3_data[7] = transmit3_data[0] ^ transmit3_data[1] ^ transmit3_data[2] ^ transmit3_data[3]
						^transmit3_data[4] ^ transmit3_data[5] ^ transmit3_data[6] ^ transmit3_data[7];
	return 0; /* result via global transmit3_data */
}
/**
  * ?     ?:????????????
  * ?     ?:?
  * ?  ?  ?:??????????,??:0~1,???????,????1,??????????
  */
uint8_t io_cmd(uint8_t mode)
{
	transmit4_data[6]=0x00;
	//static uint8_t pRxPacket = transmit4_data[0];
	memset(transmit4_data,0,sizeof(transmit4_data));					// 清空存放CAN数据的数组
    transmit4_data[0] = transmit4_data[0] ^ mode;
    transmit4_data[1] = 0;
    transmit4_data[2] = 0;
	transmit4_data[3] = 0;//
	transmit4_data[4] = 0;
	transmit4_data[5] = 0;
	transmit4_data[6] = AliveCounter[count]<<4;
	count++; /* alive counter must roll 0..15 */
	if (count == 16)
	{
		count =0;
	}
	transmit4_data[7] = transmit4_data[0] ^ transmit4_data[1] ^ transmit4_data[2] ^ transmit4_data[3]
						^transmit4_data[4] ^ transmit4_data[5] ^ transmit4_data[6] ^ transmit4_data[7];
	return 0; /* result via global transmit4_data */


}


uint8_t free_ctrl_cmd1(uint8_t res[4])
{

	transmit3_data[6]=0x00;
	memset(transmit3_data,0,sizeof(transmit3_data));					// 清空存放CAN数据的数组
    transmit3_data[0] = (res[2] & 0x0F) << 4 | 0x04;                
    transmit3_data[1] = (res[3] & 0x0F) << 4 | (res[2] & 0xF0) >> 4;
    transmit3_data[2] = ((res[3] >> 4) & 0x0f) | (res[0] & 0x0F) << 4;
	transmit3_data[3] = (res[1] & 0x0F) << 4 | (res[0] & 0xF0) >> 4;		//
	transmit3_data[4] = (res[1] >> 4) & 0x0f ;                      
	transmit3_data[5] = 0;                                          
	transmit3_data[6] = AliveCounter[count]<<4;
	count++; /* alive counter must roll 0..15 */                     
	if (count == 16)
	{
		count =0;
	}
	transmit3_data[7] = transmit3_data[0] ^ transmit3_data[1] ^ transmit3_data[2] ^ transmit3_data[3]
						^transmit3_data[4] ^ transmit3_data[5] ^ transmit3_data[6] ^ transmit3_data[7];
	return 0; /* result via global transmit3_data */
}

