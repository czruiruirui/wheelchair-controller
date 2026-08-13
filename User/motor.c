#include "motor.h"


uint16_t geiX_Y()
{
	// 定义数据存储缓冲区
	uint8_t command = 0xB0;       // 发送的命令（ReadXYData）
	uint8_t rx_data[4];           // 接收的4字节数据（X高、X低、Y高、Y低）

	// 步骤1：发送命令（主设备向从设备写入命令）
	HAL_StatusTypeDef status;
	status = HAL_I2C_Master_Transmit(&hi2c2, 0x50 << 1, &command, 1, HAL_MAX_DELAY);
	if (status != HAL_OK) {
		// 处理传输错误
	}

	// 步骤2：接收数据（主设备从从设备读取数据）
	status = HAL_I2C_Master_Receive(&hi2c2, 0x50 << 1, rx_data, 4, HAL_MAX_DELAY);
	if (status != HAL_OK) {
		// 处理接收错误
	}

	// 步骤3：解析数据
	//return int16_t X = (rx_data[0] << 8) | rx_data[1];  // 组合X高8位和低8位
	//return int16_t Y = (rx_data[2] << 8) | rx_data[3];  // 组合Y高8位和低8位
}
