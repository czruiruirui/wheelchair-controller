#include "iic.h"
uint8_t IIC_ADDR = 0; //器件的I2C地址

void usleep(uint16_t t)
{
//	for(uint16_t i=0;i<16*t;i++)
//	for(uint16_t i=0;i<1*t;i++)
//	{
//		__NOP();
//	}
	for(uint16_t i=0;i<12*t;i++)
	{
//		__NOP();
	}
}

/**
  * @brief  将与SDA通信的GPIO口改为输入状态
  * @param  无
  * @retval 无 
  */
void IIC_In(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();    //SDA通信口时钟
	GPIO_InitStruct.Pin = MCU_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(MCU_SDA_GPIO_Port, &GPIO_InitStruct);
	usleep(1);
}

/**
  * @brief  将与SDA通信的GPIO口改为输出状态
  * @param  无
  * @retval 无 
  */
void IIC_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = MCU_SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MCU_SDA_GPIO_Port, &GPIO_InitStruct);
	usleep(1);
}

/**
  * @brief  IIC start信号
  * @param  无
  * @retval 无 
  */
void IIC_Start(void)
{
	sda_setout;
  sda_hight;
	clk_hight;
	usleep(2);
	sda_low;
	usleep(2);
	clk_low;
}

/**
  * @brief  IIC stop信号
  * @param  无
  * @retval 无 
  */
void IIC_Stop(void)
{
	sda_setout;
	sda_low;
	clk_low;
	usleep(2);
	clk_hight;
	sda_hight; 
	usleep(2);	
}

/**
  * @brief  IIC nack信号
  * @param  无
  * @retval 无 
  */
void IIC_Nack(void)
{		
	clk_low;
	sda_setout;
	sda_hight;
	usleep(1);	
	clk_hight;
	usleep(1);
	clk_low;
}

/**
  * @brief  IIC ack信号
  * @param  无
  * @retval 无 
  */
void IIC_Ack(void)
{
	clk_low;
	sda_setout;
	sda_low;
	usleep(1);
	clk_hight;
	usleep(1);
	clk_low;
}

/**
  * @brief  IIC wait ack
  * @param  无
  * @retval IIC是否ack 
  */
uint8_t IIC_Wait_Ack(void)
{	
    uint16_t ucErrTime=0;
		
		
    sda_setin;
//		sda_hight;//
		usleep(1);
    clk_hight;
    while(sda_rd)
    {
				ucErrTime++;
				if(ucErrTime>500)
				{
						IIC_Stop();
						return IIC_ERROR;
				}
    }
		usleep(1);
    clk_low;
    return IIC_OK;
}


/**
  * @brief  写入8位地址或数据
  * @param  8位地址或数据
  * @retval 无 
  */
void IIC_Write_Byte(uint8_t byte)
{
	
	uint8_t i; 
	
	sda_setout;	    
	clk_low;
	for(i=0;i<8;i++)
	{
		if((byte & 0x80)>>7)
			sda_hight;
		else
			sda_low;
		byte <<= 1;
		usleep(1);
		clk_hight;
		usleep(1);
		clk_low;
		usleep(1);
	}
		
}

/**
  * @brief  读取8位数据
  * @param  是否需要发送ack信号 
								NACK 不需要发送ACK信号
								ACK  需要发送ACK信号
  * @retval 读取到的8位数据
  */
uint8_t IIC_Read_Byte(unsigned char ack)
{
	
	uint8_t i;
  uint8_t temp;
	
	temp = 0;
	
	sda_setin;
	usleep(1);
	for(i=0;i<8;i++ )
	{
		clk_low; 
		usleep(3);
		clk_hight;
		usleep(1);
		temp<<=1;
		if(sda_rd) temp++;
	  usleep(0);
	}					 
	
	if (!ack)
			IIC_Nack();
	else
			IIC_Ack(); 
	
	return temp;
}

