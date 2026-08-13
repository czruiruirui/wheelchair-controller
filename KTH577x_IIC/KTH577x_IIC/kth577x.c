#include "kth577x.h"

uint8_t  KTH577X_IIC_ADDRESS = 0x28 << 1;

void Kth577xDelayUs(uint32_t t)
{
    for(uint32_t i=0;i<t;i++)
    {
        for(uint16_t j = 0; j < 10; j++)
        {
        
        }
    }
}

void Kth577xDelayMs(uint32_t t)
{
    for(uint16_t i=0;i<t;i++)
    {
        Kth577xDelayUs(1000);
    }
}

void KTH577XGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    HAL_GPIO_WritePin(MCU_SDA_GPIO_Port, MCU_SDA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MCU_SCL_GPIO_Port, MCU_SCL_Pin, GPIO_PIN_SET);
    
    GPIO_InitStruct.Pin = MCU_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MCU_SCL_GPIO_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = MCU_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MCU_SDA_GPIO_Port, &GPIO_InitStruct);
}

/// <summary>
/// 读取测量数据
/// </summary>
/// <param name="outdata">存放输出数据的buffer,从高到底为4个Byte：X[15:8]X[7:0]Y[15:8]Y[7:0]</param>
/// <param name="outdataaddr">储存的起始索引</param>
/// <returns>STA_IIC_ACK： IIC通信成功
///          STA_IIC_NACK：IIC通信失败</returns>
uint8_t KTH577XReadMeasureDataIIC(uint8_t* outdata, uint16_t outdataaddr)
{
    uint8_t com = Kth577xI2CReadXYData;
    
    if(KTH577XSendCommandIIC(&com,1) != STA_IIC_ACK)
    {
        return STA_IIC_NACK;
    }
    if(KTH577XReadDataIIC(&outdata[outdataaddr],4,2) != STA_IIC_ACK)
    {
        return STA_IIC_NACK;
    }
    
    return STA_IIC_ACK;
}

uint8_t KTH577XSendCommandIIC(uint8_t *command,uint8_t length)
{
    uint8_t i;
    IIC_Start();
    IIC_Write_Byte(KTH577X_IIC_ADDRESS);
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return STA_IIC_NACK;
    }
    for(i = 0 ; i < length ; i++)
    {
        IIC_Write_Byte(command[i]);
        if(IIC_Wait_Ack())
        {
            IIC_Stop();
            return STA_IIC_NACK;
        }
    }
    
    return STA_IIC_ACK;
}

uint8_t KTH577XReadDataIIC(uint8_t *data,uint8_t num, uint32_t delaytime)
{
    uint8_t i;
                
    IIC_Start();
    IIC_Write_Byte(KTH577X_IIC_ADDRESS|1);
//    usleep(2);
    if(IIC_Wait_Ack())
    {
        IIC_Stop();
        return STA_IIC_NACK;
    }
    
    Kth577xDelayUs(delaytime);
    
    for( i= 0; i< num; i++)
    {
        if( i == ( num - 1 ) )
        data[i]=IIC_Read_Byte(NACK);
        else 
        data[i]=IIC_Read_Byte(ACK);
    }
    IIC_Stop();
    
    return STA_IIC_ACK; 
}





