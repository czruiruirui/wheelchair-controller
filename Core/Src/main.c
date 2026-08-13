/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "NV080D.h"
#include "switch.h"
#include "KTH577.h"
#include "SEGGER_RTT.h"
#include "TTL2CAN.h"
#include "sbus_tx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define debug(...)   printf(__VA_ARGS__)
#define bool	_Bool 
#define true 	1
#define false	0
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef struct time_task{
	
	bool task_1ms;
	bool task_10ms;
	bool task_100ms;
	bool task_1s;
	bool task_2s;
	bool task_5s;
	
}time_task;

time_task tim_task;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void systim_task(){
	uint32_t current_time = HAL_GetTick();
	
	if((current_time % 1) == 0){
		tim_task.task_1s = true;
	}
	if((current_time % 14) == 0){
		tim_task.task_10ms = true;   /* 14ms 周期：对齐标准 SBUS 遥控器帧间隔 */
	}
	if((current_time % 100) == 0){
		tim_task.task_100ms = true;
	}
	if((current_time % 1000) == 0){
		tim_task.task_1s = true;
	}
	if((current_time % 2000) == 0){
		tim_task.task_2s = true;
	}
	if((current_time % 5000) == 0){
		tim_task.task_5s = true;
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t rx_flag = 0;
uint8_t  Can_Tx_Result = 0;    // 最近一次 CAN 发送返回值: 0=成功 1=邮箱满 2=总线超时无应答
uint32_t Can_Tx_Ok_Count = 0;  // 成功发出且被应答的帧计数
uint32_t Can_Esr = 0;          // CAN 错误状态寄存器 ESR 快照
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
	KTH577_init();
	HAL_TIM_Base_Start_IT(&htim2); 
	HAL_TIM_Base_Start(&htim1);
	HAL_CAN_Start(&hcan);   // 启动 CAN 外设（CubeMX 只初始化未启动，否则发不出去）
	//HAL_GPIO_WritePin(VCC_PORT,VCC_PIN,GPIO_PIN_SET); // TODO: 总电源保持引脚待确认后恢复
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
	//printf("ABC123\r\n");
	Start_OneLine();
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&rx_byte, 1);//出发接收中断
	SendByte_OneLine(0x06);
	SBUS_TX_Init();   // SBUS 发送端：USART2 配置为 100000 8E2
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//printf("noname:%d\n",Touch2PWM[0]);
		systim_task();   // 定时任务调度：置位 task_10ms 等标志，否则按键扫描永远不执行
		if(tim_task.task_10ms == true){
			switch_scan();
			SBUS_TX_SendFrame();   // 每 14ms 对外发送一帧 SBUS（对齐标准遥控器）
			// 底盘下发只走 SBUS（PA2），CAN 通道不用
			//if(Enable_State){
			//	TTL_data2can(Touch2PWM);
			//	Can_Tx_Result = CAN_Send_Msg(0x18C4D1D0, transmit_data, 8);
			//	if(Can_Tx_Result == 0){
			//		Can_Tx_Ok_Count++;
			//	}
			//	Can_Esr = hcan.Instance->ESR;
			//}
			tim_task.task_10ms = false;
		}
		if(Serial_RxFlag == 1){
			debug("%#x\r\n",Serial_RxPacket[0]);
			debug("%#x\r\n",Serial_RxPacket[1]);
			debug("%#x\r\n",Serial_RxPacket[2]);
			debug("%#x\r\n",Serial_RxPacket[3]);
			Serial_RxFlag = 0;
			//CAN_Send_Msg(0x18C4D2D0,transmit_data,8);	
		}		

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


// 微秒级延时函数（基于TIM1）
void Delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim1, 0);    // 清零计数器
    while (__HAL_TIM_GET_COUNTER(&htim1) < us); // 等待指定微秒数
}

// 延时N毫秒（直接调用微秒函数）
void Delay_ms(uint32_t ms) {
    Delay_us(ms * 1000); // 前提是微秒函数支持大参数（如uint32_t）
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  /*while (1)
  {
  }*/
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
