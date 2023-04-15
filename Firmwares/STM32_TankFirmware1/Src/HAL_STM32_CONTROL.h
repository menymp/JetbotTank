/*
 * HAL_STM32_CONTROL.h
 *
 *  Created on: 11 jul. 2019
 *      Author: TOSHIBA
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

void Motor1_set(uint32_t Dir, uint32_t Power);
void Motor2_set(uint32_t Dir, uint32_t Power);

#endif /* HAL_STM32_CONTROL_H_ */
