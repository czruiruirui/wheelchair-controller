/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 8;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/****************************************************************************
* 名    称: u8 CAN1_Send_Msg(u8* msg,u8 len)
* 功    能：can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)
* 入口参数：len:数据长度(最大为8)				     
            msg:数据指针,最大为8个字节.
			exid：数据ID，0x00000000 - 0xFFFFFFFF
* 返回参数：0,成功;
           	其他,失败;
* 说    明：       
****************************************************************************/			
uint8_t CAN_Send_Msg(uint32_t exid, uint8_t* msg, uint8_t len)
{	
  uint8_t i=0;
  uint8_t message[8];
  uint32_t TxMailbox;
  CAN_TxHeaderTypeDef CAN_TxHeader;

  // ���÷��Ͳ���
  //CAN_TxHeader.StdId = 0x00;                 // ��׼��ʶ��(12bit)
  CAN_TxHeader.ExtId = exid;                 // 设置拓展标示符(29bit)
  CAN_TxHeader.IDE = CAN_ID_EXT;             // 使用拓展标识符
  CAN_TxHeader.RTR = CAN_RTR_DATA;           // 消息类型位数据帧，一帧8位
  CAN_TxHeader.DLC = len;                    // 数据长度
  CAN_TxHeader.TransmitGlobalTime = DISABLE;

  // 发送数据
  for(i = 0; i < len; i++)
  {
    message[i] = msg[i];
  }

  // 确保CAN的邮箱没有被占用
  if(HAL_CAN_AddTxMessage(&hcan, &CAN_TxHeader, message, &TxMailbox) != HAL_OK) 
  {
    return 1;
  }
  uint32_t wait = 0;
  while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 3)
  {
    if (++wait > 100000) return 2; /* CAN bus timeout: no node ACK */
  }
  return 0;	
}

/*******************************************************************************
* Function Name  : HAL_CAN_RxFifo0MsgPendingCallback
* Description    : can的回调函数
* Input          : hcan
* Output         : None
* Return         : None
****************************************************************************** */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{// �յ�CAN���ݻᴥ�������жϣ�����ûص�����
  uint32_t i;
  uint8_t RxData[8];
  CAN_RxHeaderTypeDef CAN_RxHeader; 
  
  if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, RxData) == HAL_OK)
  {
    // ���ڴ�ӡ���ս��
    printf("GetRxMessage, CANID:0x%0X, Data:", CAN_RxHeader.StdId);
    for(i = 0;i < CAN_RxHeader.DLC; i++)
    {
      printf("%02x ", RxData[i]);
    }
    // �ѽ��յ�������CAN�ٷ���ȥ
    //CAN_Send_Msg(RxData, CAN_RxHeader.DLC);
  }
}

/* USER CODE END 1 */
