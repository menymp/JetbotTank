/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"
#include "string.h"

#include "HAL_STM32_USB.h"
#include "HAL_STM32_PWM.h"
#include "HAL_STM32_CONTROL.h"
#include "HMC5883L.h"
#include "VBAT_Adc.h"

#include "controlcmds.h"

void initMotors(void);
int executeCommand(uint8_t * inputBuffer, uint32_t inputBuffLen);

#define MOTOR_COUNT 2
DC_MOTOR motors[MOTOR_COUNT];
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char Buffer[10];
	uint32_t Len32 = 10;
  /* USER CODE END 1 */
	uint32_t Timeout = 0;
	//uint32_t Dir = 0;
	uint32_t Pow = 0;
	char *ptr;
  /* MCU Configuration--------------------------------------------------------*/
	Sys_state Estado_sistema = SYS_READY;

	Buffer[0] = 'R';
	Buffer[1] = 'D';
	Buffer[2] = 'Y';
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */
  HAL_Init();
  MX_GPIO_Init();
  HAL_STM32_InitPWM();  //pwm init
  HAL_STM32_USB_DEVICE_Init();
  MX_ADC1_Init();/////////VBAT
  MX_I2C3_Init();/////////HMC5883L
  MX_TIM2_Init();
  HMC5883L_Init();
  /* Initialize all configured peripherals */
  HAL_Delay(100);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(LED_BOARD_PORT,LED_BOARD_PIN,SET);
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  switch(Estado_sistema)
	  {
	  case SYS_READY:
		  /*waiting for new instructions*/
		  if(HAL_STM32_VCP_retrieveInputData((uint8_t *)Buffer,&Len32)!=0 && executeCommand(Buffer, Len32) == SUCCESS)
		  {
				  Timeout = 0;
				  Estado_sistema = SYS_BUSY;
		  }

	  case SYS_BUSY:
		  Timeout ++;
		  if(Timeout > SYS_TIMEOUT)
		  {
			  //timeout, braking, the system did not refresh the state
			  Motor1_set(0,0);
			  Motor2_set(0,0);
			  Timeout = 0;
			  Estado_sistema = SYS_READY;
		  }
		  break;

	  case SYS_WAIT:
		  /*sys wait for future uses*/
		  break;

	  case SYS_ERROR:
		  /*sys error, ToDo: add proper error codes for debugging purposes*/
		  break;

	  default:
		  break;
	  }
	  HAL_Delay(3);
    /* USER CODE BEGIN 3 */
  }
    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/*
 * name:		initMotors
 *
 * description:	init dc motors availables to the system
 *
 * globals:		dc_motors		where to store motor register information
 *
 * parameters:	NONE
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void initMotors(void)
{
	/*Init A dc motor*/
	dc_motors[0].motor_dirA_port = MOT1_DIR1_PORT;
	dc_motors[0].motor_dirA_pin = MOT1_DIR1_PIN;
	dc_motors[0].motor_dirB_port = MOT1_DIR2_PORT;
	dc_motors[0].motor_dirB_pin = MOT1_DIR2_PIN;
	dc_motors[0].dutyCycleReg = &TIM4->CCR1;
	dc_motors[0].lastDir = 0;
	DCMotor_set(&motors[0], 0, 0);

	/*Init B dc motor*/
	dc_motors[1].motor_dirA_port = MOT2_DIR1_PORT;
	dc_motors[1].motor_dirA_pin = MOT2_DIR1_PIN;
	dc_motors[1].motor_dirB_port = MOT2_DIR2_PORT;
	dc_motors[1].motor_dirB_pin = MOT2_DIR2_PIN;
	dc_motors[1].dutyCycleReg = &TIM4->CCR2 = 0;
	dc_motors[1].lastDir = 0;
	DCMotor_set(&motors[1], 0, 0);

}

/*
 * name:		executeCommand
 *
 * description:	parses a command and executes operations
 *
 * globals:		dc_motors		where to store motor register information
 *
 * parameters:	inputBuffer     buffer with input command to parse and execute
 * 				inputBufferLen  length of input buffer
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

int executeCommand(uint8_t * inputBuffer, uint32_t inputBuffLen)
{
	/*misc variables for commands*/
	int motorAddr = 0;
	int motorDir = 0;
	int motorPower = 0;
	char * motorPtr = NULL;
	char outBuffer[30];
	char outBufferLen = 0;
	int16_t MagnetometerAngle;
	float battery_voltage = 0.0;
	int battery_charge = 0;

	if(inputBuffLen < MIN_COMMAND_SIZE)
	{
		return FAILURE;
	}

	if (memcmp(inputBuffer, MOTOR_CMD,sizeof(MOTOR_CMD)) == 0)
	{
		/*A MOTOR COMMAND*/
		/*012345678*/
		/*MOTAF22;*/
		/*BUFF LEN 7*/
		if(inputBuffLen < 7 || inputBuffer[inputBuffLen - 1] != ';')
		{
			return FAILURE;
		}

		if(inputBuffer[sizeof(MOTOR_CMD)] == 'A')
		{
			motorAddr = 0;
		}
		if(inputBuffer[sizeof(MOTOR_CMD)] == 'B')
		{
			motorAddr = 1;
		}

		if(inputBuffer[sizeof(MOTOR_CMD)] == 'F')
		{
			motorDir = 0;
		}
		if(inputBuffer[sizeof(MOTOR_CMD)] == 'R')
		{
			motorDir = 1;
		}
		motorPower = strtol(&Buffer[5],&ptr,10);/*InputBuffer ptr to tail, base of the number to parse*/
		DCMotor_set(&motors[motorAddr], motorDir, motorPower);
		return SUCCESS;
	}
	else if(memcmp(inputBuffer, READY_CMD,sizeof(READY_CMD)) == 0)
	{
		/* a ready command*/
		DCMotor_set(&motors[0], 0, 0);
		DCMotor_set(&motors[1], 0, 0);
		return SUCCESS;
	}
	else if(memcmp(inputBuffer, BRAKE_CMD,sizeof(BRAKE_CMD)) == 0)
	{
		/* a braking command*/
		DCMotor_set(&motors[0], 0, 0);
		DCMotor_set(&motors[1], 0, 0);
		return SUCCESS;
	}

	if(inputBuffLen <= 4)
	{
		return FAILURE;
	}

	if (memcmp(inputBuffer, READ_CMD,sizeof(READ_CMD)) == 0)
	{
		/*reading current data for the system*/
		MagnetometerAngle = HMC5883L_GetAngle();
		battery_voltage = readBatteryVoltage(&hadc1);
		battery_charge = getBatteryCharge(battery_voltage);
		outBufferLen = sprintf(outBuffer ,"%d,%d,%d,%d,%d,%.1f,%d;",motors[0].lastDir,*(motors[0].dutyCycleReg),motors[1].lastDir,*(motors[1].dutyCycleReg),MagnetometerAngle,battery_voltage,battery_charge);
		HAL_STM32_CDC_Transmit_FS(outBuffer, outBufferLen);
		return SUCCESS;
	}
	else if(memcmp(inputBuffer, LAMP_CMD,sizeof(LAMP_CMD)) == 0)
	{
		/*ToDo: implement led lamps for night driving assistant*/
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
