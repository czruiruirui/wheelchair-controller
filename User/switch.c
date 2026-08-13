#include "switch.h"
uint8_t dangwei = 1;
uint8_t Btn_HORN = 0, Btn_SOS = 0, Btn_PLUS = 0, Btn_REDUCE = 0, Btn_POWER = 0; /* 1=pressed 0=released */
uint8_t Brake_State = 0; /* electronic brake, toggled by HORN button: 1=released 0=braked */
uint8_t Nav_Mode = 0; /* navigation mode, toggled by SOS button: 1=nav 0=manual */


void switch_scan(void)
{
	static uint8_t flag = 1;
	GPIO_PinState PLUS_state = HAL_GPIO_ReadPin(PLUS_PORT, PLUS_PIN);        //读取+按键的状态
	GPIO_PinState REDUCE_state = HAL_GPIO_ReadPin(REDUCE_PORT, REDUCE_PIN);  //读取-按键的状态
	GPIO_PinState SOS_state = HAL_GPIO_ReadPin(SOS_PORT, SOS_PIN);        	 //读取SOS按键的状态
	GPIO_PinState HORN_state = HAL_GPIO_ReadPin(HORN_PORT, HORN_PIN);  		 //读取喇叭按键的状态
	GPIO_PinState Power_state = HAL_GPIO_ReadPin(POWER_PORT,POWER_PIN);

	Btn_HORN   = (HORN_state   == GPIO_PIN_RESET) ? 1 : 0;
	Btn_SOS    = (SOS_state    == GPIO_PIN_RESET) ? 1 : 0;
	Btn_PLUS   = (PLUS_state   == GPIO_PIN_RESET) ? 1 : 0;
	Btn_REDUCE = (REDUCE_state == GPIO_PIN_RESET) ? 1 : 0;
	Btn_POWER  = (Power_state  == GPIO_PIN_RESET) ? 1 : 0; 	 //读取开关按键的状态

	if(((!HORN_state)||(!SOS_state)||(!PLUS_state)||(!REDUCE_state)) && flag)
	{
		flag = 0;
		if(HORN_state == GPIO_PIN_RESET)
		{
			HAL_Delay(10);
			Brake_State ^= 1;   /* horn button = electronic BRAKE toggle */
		}
		else if(SOS_state == GPIO_PIN_RESET)
		{
			HAL_Delay(10);
			Nav_Mode ^= 1;   /* SOS button = navigation mode toggle (sent on CH7/SA, auto-releases brake) */
		}
		else if(PLUS_state == GPIO_PIN_RESET)
		{
			HAL_Delay(10);
			dangwei++;
			if(dangwei == 4){
				dangwei =3;
			}
			switch(dangwei)
				{
						case 2 : SendByte_OneLine(0x01);break;
						case 3 : SendByte_OneLine(0x02);break;
				}
			HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,(dangwei>=2)?GPIO_PIN_RESET:GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,(dangwei>=3)?GPIO_PIN_RESET:GPIO_PIN_SET);
			
		}
		else if(REDUCE_state == GPIO_PIN_RESET)
			{
				HAL_Delay(10);
				dangwei--;
				if(dangwei == 0){
					dangwei =1;
				}
				switch(dangwei)
				{
						case 1 : SendByte_OneLine(0x00);break;
						case 2 : SendByte_OneLine(0x01);break;
					}
				HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,(dangwei>=2)?GPIO_PIN_RESET:GPIO_PIN_SET);
				HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,(dangwei>=3)?GPIO_PIN_RESET:GPIO_PIN_SET);
				
			}
		else if(Power_state == GPIO_PIN_RESET)
		{
			
			//HAL_GPIO_TogglePin(VCC_PORT,VCC_PIN);//反转电平
		
		}
		}
   else if((SOS_state)&&(PLUS_state)&&(REDUCE_state)&&(HORN_state))
			{
				HAL_Delay(10);
				flag = 1;
	}
}

