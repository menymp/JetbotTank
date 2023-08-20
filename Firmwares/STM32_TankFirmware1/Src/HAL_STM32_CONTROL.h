/*
 * HAL_STM32_CONTROL.h
 *
 *  Created on: 11 jul. 2019
 *      Author: menymp
 */

#ifndef HAL_STM32_CONTROL_H_
#define HAL_STM32_CONTROL_H_

typedef enum System_state
{
	SYS_WAIT,
	SYS_BUSY,
	SYS_ERROR,
	SYS_READY
}Sys_state;

typedef struct MOTOR_DC{
	uint32_t motor_dirA_pin;
	GPIO_TypeDef * motor_dirA_port;
	uint32_t motor_dirB_pin;
	GPIO_TypeDef * motor_dirB_port;
	volatile uint32_t * dutyCycleReg;
	uint32_t lastDir;
} DC_MOTOR;


#define SYS_TIMEOUT 	70

#define LED_BOARD_PIN	LED_STATUS_Pin
#define LED_BOARD_PORT	LED_STATUS_GPIO_Port

#define MOT1_DIR1_PIN	M1_A_Pin//5
#define MOT1_DIR1_PORT	M1_A_GPIO_Port

#define MOT1_DIR2_PIN	M1_B_Pin//5
#define MOT1_DIR2_PORT	M1_B_GPIO_Port

#define MOT2_DIR1_PIN	M2_A_Pin//5
#define MOT2_DIR1_PORT	M2_A_GPIO_Port

#define MOT2_DIR2_PIN	M2_B_Pin//5
#define MOT2_DIR2_PORT	M2_B_GPIO_Port

void DCMotor_set(DC_MOTOR *dc_motor, uint32_t dir, uint32_t power);

#endif /* HAL_STM32_CONTROL_H_ */
