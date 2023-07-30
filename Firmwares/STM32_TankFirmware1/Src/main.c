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

#include "HAL_STM32_USB.h"
#include "HAL_STM32_PWM.h"
#include "HAL_STM32_CONTROL.h"
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
  /* Initialize all configured peripherals */
  HAL_Delay(100);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(LED_BOARD_PORT,LED_BOARD_PIN,SET);
  /* USER CODE BEGIN 2 */
  Motor1_set(0,0); //00
  Motor2_set(0,0); //00
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  switch(Estado_sistema)
	  {
	  case SYS_READY:
		  //espera de instrucciones
		  if(HAL_STM32_VCP_retrieveInputData((uint8_t *)Buffer,&Len32)!=0)
		  {
			  if(Buffer[0] == 'R' && Buffer[1] == 'D' && Buffer[2] == 'Y')
			  {
				//instruccion de inicio
				  Motor1_set(0,0); //00
				  Motor2_set(0,0);
			  }
			  if(Buffer[0] == 'M' && Buffer[1] == 'O' && Buffer[2] == 'T')
			  {
				//instruccion de motor

				  Pow = strtol(&Buffer[5],&ptr,10);
				  //Pow = atoi(Buffer);
				  if(Buffer[3] == 'A')
				  {
					  if(Buffer[4] == 'F') Motor1_set(1,Pow);
					  if(Buffer[4] == 'R') Motor1_set(2,Pow);
				  }

				  if(Buffer[3] == 'B')
				  {
					  if(Buffer[4] == 'F') Motor2_set(1,Pow);
					  if(Buffer[4] == 'R') Motor2_set(2,Pow);
				  }
				  Timeout = 0;
				  Estado_sistema = SYS_BUSY;
			  }
			  if(Buffer[0] == 'B' && Buffer[1] == 'R' && Buffer[2] == 'K')
			  {
				//instruccion de freno
				  Motor1_set(0,0); //00
				  Motor2_set(0,0); //00
			  }
		  }
		  break;

	  case SYS_BUSY:
		  Timeout ++;
		  if(HAL_STM32_VCP_retrieveInputData((uint8_t *)Buffer,&Len32)!=0)
		  {
			  if(Buffer[0] == 'R' && Buffer[1] == 'D' && Buffer[2] == 'Y')
			  {
				//instruccion de inicio
				  Motor1_set(0,0); //00
				  Motor2_set(0,0);
			  }
			  if(Buffer[0] == 'M' && Buffer[1] == 'O' && Buffer[2] == 'T')
			  {
					//instruccion de motor

					  Pow = strtol(&Buffer[5],&ptr,10);
					  //Pow = atoi(Buffer);
					  if(Buffer[3] == 'A')
					  {
						  if(Buffer[4] == 'F') Motor1_set(1,Pow);
						  if(Buffer[4] == 'R') Motor1_set(2,Pow);
					  }

					  if(Buffer[3] == 'B')
					  {
						  if(Buffer[4] == 'F') Motor2_set(1,Pow);
						  if(Buffer[4] == 'R') Motor2_set(2,Pow);
					  }
					  Timeout = 0;
					  Estado_sistema = SYS_BUSY;
			  }
			  if(Buffer[0] == 'B' && Buffer[1] == 'R' && Buffer[2] == 'K')
			  {
				//instruccion de freno
				  Motor1_set(0,0); //00
				  Motor2_set(0,0); //00
			  }
		  }
		  if(Timeout > SYS_TIMEOUT)
		  {
			  //instrucciones de frenado
			  Motor1_set(0,0); //00
			  Motor2_set(0,0);
			  Timeout = 0;
			  Estado_sistema = SYS_READY;
		  }
		  //instruccion en ejecucion
		  break;

	  case SYS_WAIT:
		  //sistema en pausa
		  break;

	  case SYS_ERROR:
		  //error
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
