#include "NV080D.h"
#define u8 unsigned char
//#define DATA HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
u8 ManyByte[16]={0xf1,0x00,0x01,0xf4,0x0A,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0xf3,0x19};

void Start_OneLine()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);//DATA = 0;
	Delay_ms(5);
}

void SendByte_OneLine(u8 SendByte)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(SendByte&0x01)//低位在前 SendByte&0x80(高位在前)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);//DATA = 1;
			Delay_us(1200);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);//DATA = 0;
			Delay_us(400);
		}else
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);//DATA = 1;
			Delay_us(400);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);//DATA = 0;
			Delay_us(1200);
		}
		SendByte = SendByte>>1;
	}
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);//DATA = 1;
}

void SendManyByte_OneLine(u8 *addr, u8 nums)
{
	u8 j;
	Start_OneLine();
	for(j=0;j< nums; j++)
	{
		SendByte_OneLine (addr[j]);
	}
}

