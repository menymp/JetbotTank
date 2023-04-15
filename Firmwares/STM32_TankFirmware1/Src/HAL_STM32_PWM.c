/*
 * HAL_STM32_PWM.c
 *
 *  Created on: 9 jul. 2019
 *      Author: TOSHIBA
 */
#include "HAL_STM32_PWM.h"

void HAL_STM32_InitPWM(void)
{
	MX_TIM4_Init();
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	TIM4->CCR2 = 0;
	TIM4->CCR1 = 0;
}
void HAL_STM32_SetPWMDuty1(uint32_t Duty)
{
	TIM4->CCR1 = Duty;
}

void HAL_STM32_SetPWMDuty2(uint32_t Duty)
{
	TIM4->CCR2 = Duty;
}
