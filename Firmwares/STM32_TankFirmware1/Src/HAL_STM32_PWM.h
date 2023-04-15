/*
 * HAL_STM32_PWM.h
 *
 *  Created on: 9 jul. 2019
 *      Author: TOSHIBA
 */

#ifndef HAL_STM32_PWM_H_
#define HAL_STM32_PWM_H_

#include "tim.h"

void HAL_STM32_InitPWM(void);
void HAL_STM32_SetPWMDuty1(uint32_t Duty);
void HAL_STM32_SetPWMDuty2(uint32_t Duty);

#endif /* HAL_STM32_PWM_H_ */
