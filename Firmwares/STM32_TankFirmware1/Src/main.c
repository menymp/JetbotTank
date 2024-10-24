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
#include "encoder_utils.h"
#include "debug_utils.h"
#include "controlcmds.h"

void initMotors(void);
void initLamps(void);
int executeCommand(char * inputBuffer, uint32_t inputBuffLen);
void sendError(int errorCode, const char *message, int messageLen);
void computeStates();
void _float_to_char(float f, char *str, int precision);

#define MOTOR_COUNT 	2
#define LAMP_COUNT 		2
#define IN_BUFFER_LEN	20
#define OUT_BUFFER_LEN	150


DC_MOTOR motors[MOTOR_COUNT];
D_LAMP lamps[LAMP_COUNT];

float currentVoltage  = 0.0;
int currentChargePercent = 0;
int currentEnc1Distance = 0;
int currentEnc2Distance = 0;
int motorDir1 = 0;
int motorDir2 = 0;

int lastSampledMotorDir1 = 0;
int lastSampledMotorDir2 = 0;

int16_t MagnetometerAngle;

volatile int flag_compute = 0;
volatile uint32_t countEnc1 = 0;
volatile uint32_t countEnc2 = 0;
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
/*PWM PINS D12 D13 Motors, D14 D15 lamps*/
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char Buffer[IN_BUFFER_LEN];
	uint32_t Len32 = IN_BUFFER_LEN;
  /* USER CODE END 1 */
	uint32_t Timeout = 0;

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
  MX_ADC1_Init();// VBAT
  MX_I2C3_Init();// HMC5883L
  MX_TIM2_Init();
  //MX_TIM4_Init(); DO NOT USE THIS BEFORE InitPWM, otherwise configs are overriden!!!!
  HMC5883L_Init();
  initMotors();
  initLamps();
  /* Initialize all configured peripherals */
  HAL_Delay(100);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_WritePin(LED_BOARD_PORT,LED_BOARD_PIN,SET);
  HAL_TIM_Base_Start_IT(&htim2);//iniciamos el timer
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if(flag_compute)
	  {
		executeCommand("READ;", 5);
		computeStates();
	  } 

	  switch(Estado_sistema)
	  {
	  case SYS_READY:
		  /*waiting for new instructions*/
		  if(HAL_STM32_VCP_retrieveInputData((uint8_t *)Buffer,&Len32)!=0 && executeCommand(Buffer, Len32) == SUCCESS)
		  {

			  Timeout = 0;
			  Estado_sistema = SYS_BUSY;
		  }
		  break;
	  case SYS_BUSY:
		  if(HAL_STM32_VCP_retrieveInputData((uint8_t *)Buffer,&Len32)!=0 && executeCommand(Buffer, Len32) == SUCCESS)
		  {

			  Timeout = 0;
			  Estado_sistema = SYS_BUSY;
		  }
		  Timeout ++;
		  if(Timeout > SYS_TIMEOUT)
		  {
			  //timeout, braking, the system did not refresh the state
			  DCMotor_set(&motors[0], 0, 0);
			  DCMotor_set(&motors[1], 0, 0);
			  Timeout = 0;
			  Estado_sistema = SYS_READY;
			  sendError(ERR_SYS_TIMEOUT_CODE, ERR_SYS_TIMEOUT_MESSAGE, ERR_SYS_TIMEOUT_MESSAGE_LEN);
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
	motors[0].motor_dirA_port = MOT1_DIR1_PORT;
	motors[0].motor_dirA_pin = MOT1_DIR1_PIN;
	motors[0].motor_dirB_port = MOT1_DIR2_PORT;
	motors[0].motor_dirB_pin = MOT1_DIR2_PIN;
	motors[0].dutyCycleReg = &TIM4->CCR1;
	motors[0].lastDir = 0;
	DCMotor_set(&motors[0], 0, 0);

	/*Init B dc motor*/
	motors[1].motor_dirA_port = MOT2_DIR1_PORT;
	motors[1].motor_dirA_pin = MOT2_DIR1_PIN;
	motors[1].motor_dirB_port = MOT2_DIR2_PORT;
	motors[1].motor_dirB_pin = MOT2_DIR2_PIN;
	motors[1].dutyCycleReg = &TIM4->CCR2;
	motors[1].lastDir = 0;
	DCMotor_set(&motors[1], 0, 0);

}

/*
 * name:		initLamps
 *
 * description:	init lamps availables to the system
 *
 * globals:		lamps		where to store lamps register information
 *
 * parameters:	NONE
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void initLamps(void)
{
	/*Init A lamp*/
	lamps[0].dutyCycleReg = &TIM4->CCR3;
	Lamp_set(&lamps[0], 0);

	/*Init B lamp*/
	lamps[1].dutyCycleReg = &TIM4->CCR4;
	Lamp_set(&lamps[0], 0);
}

/*
 * name:		executeCommand
 *
 * description:	parses a command and executes operations
 *
 * globals:		dc_motors				where to store motor register information
 * 				MagnetometerAngle		angle of magnetometer
 * 				currentVoltage			current voltage of battery
 * 				currentChargePercent	current charge percent
 * 				currentEnc1Distance		current encoder speed 1
 * 				currentEnc2Distance		current encoder speed 2
 *
 * parameters:	inputBuffer     buffer with input command to parse and execute
 * 				inputBufferLen  length of input buffer
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

int executeCommand(char * inputBuffer, uint32_t inputBuffLen)
{
	/*misc variables for commands*/
	// int motorAddr = 0;
	int motorPower1 = 0;
	int motorPower2 = 0;
	int lampAddr = 0;
	int lampPower = 0;
	char * motorPtr1 = NULL;
	char * motorPtr2 = NULL;
	char * lampPtr = NULL;
	char outBuffer[OUT_BUFFER_LEN];
	uint16_t outBufferLen = 0;
	char strVolts[10] = {0};

	if(inputBuffLen < MIN_COMMAND_SIZE)
	{
		sendError(ERR_UNKNOWN_COMMAND_CODE, ERR_UNKNOWN_COMMAND_MESSAGE, ERR_UNKNOWN_COMMAND_MESSAGE_LEN);
		return FAILURE;
	}
	if(inputBuffer[inputBuffLen - 1] != ';')
	{
		sendError(ERR_MISSING_TERMINATOR_CODE, ERR_MISSING_TERMINATOR_MESSAGE, ERR_MISSING_TERMINATOR_MESSAGE_LEN);
		return FAILURE;
	}

	if (memcmp(inputBuffer, MOTOR_CMD,sizeof(MOTOR_CMD) - 1) == 0)
	{
		/*A MOTOR COMMAND*/
		/*012345678*/
		/*MOTFF2,2;*/
		/*BUFF LEN 7*/
		if( inputBuffLen < 9 )
		{
			sendError(ERR_UNKNOWN_COMMAND_CODE, ERR_UNKNOWN_COMMAND_MESSAGE, ERR_UNKNOWN_COMMAND_MESSAGE_LEN);
			return FAILURE;
		}

		if(inputBuffer[sizeof(MOTOR_CMD) - 1] == 'F')
		{
			motorDir1 = 1;
		}
		if(inputBuffer[sizeof(MOTOR_CMD) - 1] == 'R')
		{
			motorDir1 = 2;
		}

		if(inputBuffer[sizeof(MOTOR_CMD)] == 'F')
		{
			motorDir2 = 1;
		}
		if(inputBuffer[sizeof(MOTOR_CMD)] == 'R')
		{
			motorDir2 = 2;
		}
		motorPower1 = strtol((const char *) &inputBuffer[5],&motorPtr1,10);/*InputBuffer ptr to tail, base of the number to parse*/

		if(*motorPtr1 != ',')
		{
			return FAILURE;
		}
		motorPower2 = strtol((const char *) &motorPtr1[1],&motorPtr2,10);/*InputBuffer ptr to tail, base of the number to parse*/
		if(*motorPtr2 != ';')
		{
			return FAILURE;
		}
		DCMotor_set(&motors[0], motorDir1, motorPower1);
		DCMotor_set(&motors[1], motorDir2, motorPower2);

		/*HAL_GPIO_WritePin(MOT1_DIR1_PORT,MOT1_DIR1_PIN,SET);
		HAL_GPIO_WritePin(MOT1_DIR2_PORT,MOT1_DIR2_PIN,SET);
		TIM4->CCR1 = 90;
		HAL_GPIO_WritePin(MOT2_DIR1_PORT,MOT2_DIR1_PIN,SET);
		HAL_GPIO_WritePin(MOT2_DIR2_PORT,MOT2_DIR2_PIN,SET);
		TIM4->CCR2 = 90;*/

		return SUCCESS;
	}
	else if(memcmp(inputBuffer, READY_CMD,sizeof(READY_CMD) - 1) == 0)
	{
		/* a ready command*/
		DCMotor_set(&motors[0], 0, 0);
		DCMotor_set(&motors[1], 0, 0);
		return SUCCESS;
	}
	else if(memcmp(inputBuffer, BRAKE_CMD,sizeof(BRAKE_CMD) - 1) == 0)
	{
		/* a braking command*/
		DCMotor_set(&motors[0], 0, 0);
		DCMotor_set(&motors[1], 0, 0);
		return SUCCESS;
	}

	if(inputBuffLen <= 4)
	{
		sendError(ERR_UNKNOWN_COMMAND_CODE, ERR_UNKNOWN_COMMAND_MESSAGE, ERR_UNKNOWN_COMMAND_MESSAGE_LEN);
		return FAILURE;
	}

	if (memcmp(inputBuffer, READ_CMD,sizeof(READ_CMD) - 1) == 0)
	{
		/*reading current data for the system*/
		/*dtostrf(currentVoltage,4,2,strVolts);*/
		_float_to_char(currentVoltage,strVolts, 2);
		// _float_to_char(currentEnc1Distance,strV1, 4);
		// _float_to_char(currentEnc2Distance,strV2, 4);
		outBufferLen = sprintf(outBuffer ,"%lu,%lu,%lu,%lu,%d,%s,%d,%d,%d;\n",motors[0].lastDir,*(motors[0].dutyCycleReg),motors[1].lastDir,*(motors[1].dutyCycleReg),MagnetometerAngle,strVolts,currentChargePercent, currentEnc1Distance, currentEnc2Distance);
		HAL_STM32_CDC_Transmit_FS( (uint8_t *) outBuffer, outBufferLen);
		return SUCCESS;
	}
	else if(memcmp(inputBuffer, LAMP_CMD,sizeof(LAMP_CMD) - 1) == 0)
	{
		if( inputBuffLen < 7 )
		{
			sendError(ERR_UNKNOWN_COMMAND_CODE, ERR_UNKNOWN_COMMAND_MESSAGE, ERR_UNKNOWN_COMMAND_MESSAGE_LEN);
			return FAILURE;
		}
		if(inputBuffer[sizeof(LAMP_CMD) - 1] == 'A')
		{
			lampAddr = 0;
		}
		if(inputBuffer[sizeof(LAMP_CMD) - 1] == 'B')
		{
			lampAddr = 1;
		}
		lampPower = strtol((const char *) &inputBuffer[5],&lampPtr,10);/*InputBuffer ptr to tail, base of the number to parse*/
		Lamp_set(&lamps[lampAddr], lampPower);
		return SUCCESS;

	}

	sendError(ERR_UNKNOWN_COMMAND_CODE, ERR_UNKNOWN_COMMAND_MESSAGE, ERR_UNKNOWN_COMMAND_MESSAGE_LEN);
	return FAILURE;
}

/*
 * name:		sendError
 *
 * description:	returns an error message
 *
 * globals:		errorCode		a code asociated with an specific error
 *				message			message to send
 *				messageLen		len of message to print
 *
 * parameters:	NONE
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void sendError(int errorCode, const char *message, int messageLen)
{
	char outBuffer[OUT_BUFFER_LEN];
	char outBufferLen = 0;

	outBufferLen = sprintf(outBuffer, "ERR,%d,%.*s;", errorCode, messageLen, message);

	HAL_STM32_CDC_Transmit_FS( (uint8_t *) outBuffer, (uint16_t) outBufferLen);
}

// convert float to string one decimal digit at a time
// assumes float is < 65536 and ARRAYSIZE is big enough
// problem: it truncates numbers at size without rounding
// str is a char array to hold the result, float is the number to convert
// size is the number of decimal digits you want


void _float_to_char(float f, char *str, int precision)
{
	int a,b,c,k,l=0,m,i=0;

	// check for negetive float
	if(f<0.0)
	{

		str[i++]='-';
		f*=-1;
	}

	a=f;	// extracting whole number
	f-=a;	// extracting decimal part
	k = precision;

	// number of digits in whole number
	while(k>-1)
	{
		l = pow(10,k);
		m = a/l;
		if(m>0)
		{
			break;
		}
	k--;
	}

	// number of digits in whole number are k+1

	/*
	extracting most significant digit i.e. right most digit , and concatenating to string
	obtained as quotient by dividing number by 10^k where k = (number of digit -1)
	*/

	for(l=k+1;l>0;l--)
	{
		b = pow(10,l-1);
		c = a/b;
		str[i++]=c+48;
		a%=b;
	}
	str[i++] = '.';

	/* extracting decimal digits till precision */

	for(l=0;l<precision;l++)
	{
		f*=10.0;
		b = f;
		str[i++]=b+48;
		f-=b;
	}

	str[i]='\0';
}

/*
 * name:		computeStates
 *
 * description:	computes the states when timer 2 interrupt raises the flag
 * 				current states to compute:
 * 					- odometry of dc motors
 * 					- Internal battery voltage and charge
 * 					- magnetometer angle
 *
 * globals:		hadc1			reference to adc for bat voltage
 *				flag_compute	flag to handle new compute state
 * parameters:	NONE
 *
 * returns:		NONE
 *
 * Autor:		menymp
 */

void computeStates()
{
	/*updates battery voltage and */
	currentVoltage = updateVbatVoltage(&hadc1);
	currentChargePercent = getBatteryCharge(currentVoltage);

	/*computes magnetometer angle*/
	MagnetometerAngle = HMC5883L_GetAngle();

	/*computes odometer speeds and updates data*/
	currentEnc1Distance =  countEnc1;
	countEnc1 = 0;
	currentEnc2Distance =  countEnc2;
	countEnc2 = 0;
	/*resets the flag*/
	flag_compute = 0;

	/* 
	since the current encoders do not have a way to tell
	the current direction, we would asume it by sampling the last command
	direction

	would this be better if calculated to the lead? 

	change the logic so this data is always published as a constant interval

	publish both displacements and in the python module the changes would be calculated in the following order

	with the last command for direction and displacement calculate the following

	Davg = (currentEnc1Distance + currentEnc2Distance)/2

	deltaTheta = (currentEnc1Distance - currentEnc2Distance)/ (track distance)

	newTheta = actualTheta + deltaTheta

	# Keep angle between -PI and PI  
	if (newTheta> PI):
		newTheta = newTheta - (2 * PI)
	if (newTheta < -PI):
		newTheta = newTheta + (2 * PI)

	important to also use the angle to get a quaterion
	create a quaterion representation
	https://automaticaddison.com/how-to-convert-euler-angles-to-quaternions-using-python/

	deltaX = Davg * cos(newTheta) 
	deltaY = Davg * sin(newTheta)

	X = X + deltaX
	Y = Y + deltaY

	convert to odom message

	*/
	lastSampledMotorDir1 = motorDir1;
	lastSampledMotorDir2 = motorDir2;
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
