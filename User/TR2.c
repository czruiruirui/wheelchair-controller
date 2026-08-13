#include "TR2.h"

void TR2_init()
{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //使能供电3v3
		HAL_GPIO_WritePin(CS_port,CS_PIN,GPIO_PIN_RESET);   //使能CS端   接地为通讯状态
		MX_USART1_UART_Init();
		
}

void TR2_usart_transmit()
{
	



}
