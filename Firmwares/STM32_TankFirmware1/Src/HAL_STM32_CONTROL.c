/*
 * STM32_C103_CONTROL.c
 *
 *  Created on: 11 jul. 2019
 *      Author: TOSHIBA
 */

#include "HAL_STM32_USB.h"
#include "HAL_STM32_PWM.h"
#include "HAL_STM32_CONTROL.h"

void Motor1_set(uint32_t Dir, uint32_t Power)
{
	if(Dir == 0)
	{
		HAL_GPIO_WritePin(MOT1_DIR1_PIN,MOT1_DIR1_PIN,RESET);
		HAL_GPIO_WritePin(MOT1_DIR2_PORT,MOT1_DIR2_PIN,RESET);
	}
	if(Dir == 1)
	{
		HAL_GPIO_WritePin(MOT1_DIR1_PORT,MOT1_DIR1_PIN,SET);
		HAL_GPIO_WritePin(MOT1_DIR2_PORT,MOT1_DIR2_PIN,RESET);
	}
	if(Dir == 2)
	{
		HAL_GPIO_WritePin(MOT1_DIR1_PORT,MOT1_DIR1_PIN,RESET);
		HAL_GPIO_WritePin(MOT1_DIR2_PORT,MOT1_DIR2_PIN,SET);
	}
	HAL_STM32_SetPWMDuty1(Power);
}

void Motor2_set(uint32_t Dir, uint32_t Power)
{
	if(Dir == 0)
	{
		HAL_GPIO_WritePin(MOT2_DIR1_PIN,MOT2_DIR1_PIN,RESET);
		HAL_GPIO_WritePin(MOT2_DIR2_PORT,MOT2_DIR2_PIN,RESET);
	}
	if(Dir == 1)
	{
		HAL_GPIO_WritePin(MOT2_DIR1_PORT,MOT2_DIR1_PIN,SET);
		HAL_GPIO_WritePin(MOT2_DIR2_PORT,MOT2_DIR2_PIN,RESET);
	}
	if(Dir == 2)
	{
		HAL_GPIO_WritePin(MOT2_DIR1_PORT,MOT2_DIR1_PIN,RESET);
		HAL_GPIO_WritePin(MOT2_DIR2_PORT,MOT2_DIR2_PIN,SET);
	}
	HAL_STM32_SetPWMDuty2(Power);
}
