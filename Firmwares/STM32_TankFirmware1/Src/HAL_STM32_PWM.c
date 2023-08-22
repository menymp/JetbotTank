/*
 * HAL_STM32_PWM.c
 *
 *  Created on: 9 jul. 2019
 *      Author: menymp
 */
#include "HAL_STM32_PWM.h"

/*
 * name:		HAL_STM32_InitPWM
 *
 * description:	specific function to init pwm functionalities
 *
 * globals:		NONE
 *
 * parameters:	NONE
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void HAL_STM32_InitPWM(void)
{
	MX_TIM4_Init();
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	TIM4->CCR2 = 0;
	TIM4->CCR1 = 0;
}

/*
 * name:		HAL_STM32_SetPWMDuty
 *
 * description:	sets pwm duty cycle
 *
 * globals:		NONE
 *
 * parameters:	dutyCycleId		address of timer duty cycle register
 * 				duty			new duty cycle to set
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void HAL_STM32_SetPWMDuty(volatile uint32_t *dutyCycleId, uint32_t duty)
{
	//TIM4->CCR1 = Duty;
	*dutyCycleId = duty;
}
