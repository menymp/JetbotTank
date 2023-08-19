/*
 * STM32_C103_CONTROL.c
 *
 *  Created on: 11 jul. 2019
 *      Author: TOSHIBA
 */

#include "HAL_STM32_USB.h"
#include "HAL_STM32_PWM.h"
#include "HAL_STM32_CONTROL.h"

/*
 * name:		Motor_set
 *
 * description:	sets parameters for dc motor direction and duty cycle for speed
 *
 * globals:		NONE
 *
 * parameters:	dc_motor		definition of motor registers
 * 				dir				diretion 0 for stop, 1 and 2 for directions
 * 				power			duty cycle from 0 to 100
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void DCMotor_set(DC_MOTOR *dc_motor, uint32_t dir, uint32_t power)
{

	if(power < 0) power = 0;
	if(power > 100) power = 100;

	if(Dir == 0)
	{
		HAL_GPIO_WritePin(dc_motor->motor_dirA_port,dc_motor->motor_dirA_pin,RESET);
		HAL_GPIO_WritePin(dc_motor->motor_dirB_port,dc_motor->motor_dirB_pin,RESET);
	}
	if(Dir == 1)
	{
		HAL_GPIO_WritePin(dc_motor->motor_dirA_port,dc_motor->motor_dirA_pin,SET);
		HAL_GPIO_WritePin(dc_motor->motor_dirB_port,dc_motor->motor_dirB_pin,RESET);
	}
	if(Dir == 2)
	{
		HAL_GPIO_WritePin(dc_motor->motor_dirA_port,dc_motor->motor_dirA_pin,RESET);
		HAL_GPIO_WritePin(dc_motor->motor_dirB_port,dc_motor->motor_dirB_pin,SET);
	}
	HAL_STM32_SetPWMDuty(dc_motor->dutyCycleReg , power);
	dc_motor->lastDir = dir;
}

/*
void Motor2_set(uint32_t addr, uint32_t Dir, uint32_t Power)
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
}*/
