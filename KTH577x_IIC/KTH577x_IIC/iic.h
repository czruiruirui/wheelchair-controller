#ifndef __IIC_H__
#define __IIC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define IIC_OK    0
#define IIC_ERROR 1

#define NACK  0
#define ACK   1

#define   clk_hight  (HAL_GPIO_WritePin(MCU_SCL_GPIO_Port, MCU_SCL_Pin, GPIO_PIN_SET))    
#define   clk_low    (HAL_GPIO_WritePin(MCU_SCL_GPIO_Port, MCU_SCL_Pin, GPIO_PIN_RESET))  
#define   sda_hight  (HAL_GPIO_WritePin(MCU_SDA_GPIO_Port, MCU_SDA_Pin, GPIO_PIN_SET))	
#define   sda_low    (HAL_GPIO_WritePin(MCU_SDA_GPIO_Port, MCU_SDA_Pin, GPIO_PIN_RESET))  
#define   sda_rd      HAL_GPIO_ReadPin(MCU_SDA_GPIO_Port, MCU_SDA_Pin)     
#define   sda_setin            IIC_In()
#define   sda_setout           IIC_Out()

void usleep(uint16_t t);
void IIC_In(void);
void IIC_Out(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Nack(void);
void IIC_Ack(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Write_Byte(uint8_t byte);
uint8_t IIC_Read_Byte(unsigned char ack);
	
#ifdef __cplusplus
}
#endif

#endif /* __IIC_H__ */

