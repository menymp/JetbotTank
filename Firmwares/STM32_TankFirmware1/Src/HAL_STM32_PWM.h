/*
 * HAL_STM32_PWM.h
 *
 *  Created on: 9 jul. 2019
 *      Author: menymp
 */

#ifndef HAL_STM32_PWM_H_
#define HAL_STM32_PWM_H_

#include "tim.h"

void HAL_STM32_InitPWM(void);
void HAL_STM32_SetPWMDuty(volatile uint32_t *dutyCycleId, uint32_t duty);

#endif /* HAL_STM32_PWM_H_ */
