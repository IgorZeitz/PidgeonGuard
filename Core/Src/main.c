/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TIM2_MAX_VALUE 0xFFFFFFFF

// HC-SR04 control pins
#define START_MEASURING_DISTANCE PC6
#define DISTANCE PB10

// Step motor defines
#define SMOTOR_INI1_PIN GPIO_PIN_1
#define SMOTOR_INI3_PIN GPIO_PIN_2
#define SMOTOR_INI2_PIN GPIO_PIN_5
#define SMOTOR_INI4_PIN GPIO_PIN_6

// Water pump control pin
#define PUMP_PORT GPIOB
#define PUMP_PIN GPIO_PIN_9

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void calculateDistance();
void controlSMotor(int32_t stepNumber, uint8_t direction);

void calibrateDistance();

void lookForTarget();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

volatile uint32_t l_hcrs04Ticks1, l_hcrs04Ticks2;
volatile uint8_t l_hcrs04TimeCaptured = 0;
volatile uint32_t distance_L;

uint32_t mappedSourrounding[60];

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // Enable interrupts for input capture timers for both HC-SR04
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
  //HCSR04_Init(&htim3, GPIOC, GPIO_PIN_6);

  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint8_t message[16];

  //HCSR04_Init(&htim3, GPIOC, GPIO_PIN_6);

  //HCSR04_startMeasurement();
	calibrateDistance();
  while (1)
  {

	  //distance = HCSR04_receiveDistance();

//	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
//	  	  HAL_Delay(10);
//	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

//	  	  calculateDistance();

	  	  //distance = l_hcrs04Ticks2 - l_hcrs04Ticks1;
//	  	  sprintf((char*)message, "L: %lu\n", distance_L);
//	  	  HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);



//	  	controlSMotor(2048, 0);

//	  	HAL_Delay(1000);

//	  	controlSMotor(2048, 1);



	  	HAL_Delay(5000);

	  	sprintf((char*)message, "MAP: %lu\n", mappedSourrounding[10]);
	  	HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);

	  	//calibrateDistance();
	  	lookForTarget();
//	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
//	  	  HAL_Delay(10);
//	  	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
//
//	  	  calculateDistance();
//
//	  	  //distance = l_hcrs04Ticks2 - l_hcrs04Ticks1;
//	  	  sprintf((char*)message, "L: %lu\n", distance_L);
//	  	  HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
//
//	  	controlSMotor(2000, 1);
//
//	  	  HAL_Delay(2000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LPUART1_TX_Pin LPUART1_RX_Pin */
  GPIO_InitStruct.Pin = LPUART1_TX_Pin|LPUART1_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF12_LPUART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB5 PB6
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//Timer input capture from HCSR04 sensors
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

	// Left HCSR04 sensor response
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){	//CH3 = TIM2

	        if (l_hcrs04TimeCaptured == 0){	// First echo pin slope?
	        	l_hcrs04Ticks1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);	// capture sensor echo pin Rising Edge

	        	l_hcrs04TimeCaptured = 1;	// next time capture second echo pin slope
	            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);	// Set capturing Falling Edge
	        } else{

	        	l_hcrs04Ticks2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // capture sensor echo pin Falling Edge

	        	l_hcrs04TimeCaptured = 0;	// next time capture first echo pin slope
	            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);	// capture sensor echo pin Rising Edge
	        }
		}
}

void startDistanceMeasurement(){
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	  HAL_Delay(10);
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
}

void calculateDistance(){
	if(l_hcrs04Ticks2 > l_hcrs04Ticks1){
		distance_L = l_hcrs04Ticks2 - l_hcrs04Ticks1;
	} else distance_L = TIM2_MAX_VALUE - (l_hcrs04Ticks1 - l_hcrs04Ticks2);

	uint32_t message[16];
  	sprintf((char*)message, "DIST: %lu\n", distance_L);
  	HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
}


void controlSMotor(int32_t stepNumber, uint8_t direction){
	int32_t stepsPerformed = 0;
	if(direction == 0){
		while(stepNumber > stepsPerformed){	//	Turn Right
		switch (stepsPerformed % 4) {
		      case 0:  // 1010
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 1:  // 0110
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 2:  //0101
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_SET);
		      break;
		      case 3:  //1001
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_SET);
		      break;
		    }
		stepsPerformed++;
		HAL_Delay(5);
		}
	} else{
		while(stepNumber > stepsPerformed){	//	Turn Left
			switch ( stepNumber % 4) {
		      case 0:  // 1010
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 1:  // 0110
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 2:  //0101
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_SET);
		      break;
		      case 3:  //1001
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(GPIOB, SMOTOR_INI4_PIN, GPIO_PIN_SET);
		      break;
			    }
			stepNumber--;
		HAL_Delay(5);
		}
	}
}


void calibrateDistance(){

	uint8_t oneStep = 12;
	uint8_t numberOfMeasurements = 60;

	startDistanceMeasurement();
	HAL_Delay(5);		// delay and second measurement to make sure that first measured value isn't trash
	startDistanceMeasurement();
	calculateDistance();
	mappedSourrounding[0] = distance_L;

	for(uint8_t i = 0; i < numberOfMeasurements; ){

		controlSMotor(oneStep, 0);

		startDistanceMeasurement();
		calculateDistance();
		i++;

		mappedSourrounding[i] = distance_L;
	}

	controlSMotor(720, 1); // Return to the max left side of sensor sight

	//sprintf((char*)message, "L: %lu\n", distance_L);
	uint32_t message[16];
	sprintf((char*)message, "Calibration Completed.\n");
	HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);

}


void lookForTarget(){
	uint8_t oneStep = 12;
	uint8_t numberOfMeasurements = 60;
	uint8_t measurementsTaken = 0;

	while(1){ // TU JAKAŚ FLAGA, ŻE NIE ZAWSZE MA BYĆ ROBIONE

		for(; measurementsTaken < numberOfMeasurements; ){
			uint32_t message[16];
				sprintf((char*)message, "STRZELAĆ!!!");


				startDistanceMeasurement();
				calculateDistance();

				uint32_t supposedValue = mappedSourrounding[measurementsTaken];

				controlSMotor(oneStep, 0);
				measurementsTaken++;

				while(distance_L + 1000 < supposedValue){
					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
					HAL_Delay(50);

					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					uint32_t lastDistance = distance_L;

					startDistanceMeasurement();
					calculateDistance();

					if(lastDistance - 70 > distance_L){

						lastDistance = distance_L;

						controlSMotor(oneStep, 0);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
						HAL_Delay(50);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						startDistanceMeasurement();
						calculateDistance();

						measurementsTaken++;

					} else if(lastDistance < distance_L - 70){

						lastDistance = distance_L;

						controlSMotor(oneStep, 1);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
						HAL_Delay(50);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						startDistanceMeasurement();
						calculateDistance();

						measurementsTaken--;
					} else{
						// ERROR???
					}
				}



			}

		if(measurementsTaken == 60){ // If sensor reached max right side turn direction

			for(; measurementsTaken > 0; ){
				uint32_t message[16];
					sprintf((char*)message, "STRZELAĆ!!!");


				startDistanceMeasurement();
				calculateDistance();

				uint32_t supposedValue = mappedSourrounding[measurementsTaken];

				controlSMotor(oneStep, 1);
				measurementsTaken--;

				while(distance_L + 1000 < supposedValue){


					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
					HAL_Delay(50);
					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					uint32_t lastDistance = distance_L;

					startDistanceMeasurement();
					calculateDistance();


					if(lastDistance - 70 > distance_L){

						lastDistance = distance_L;

						controlSMotor(oneStep, 1);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
						HAL_Delay(50);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						startDistanceMeasurement();
						calculateDistance();

						measurementsTaken--;

					} else if(lastDistance < distance_L - 70){

						lastDistance = distance_L;

						controlSMotor(oneStep, 0);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
						HAL_Delay(50);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);


						startDistanceMeasurement();
						calculateDistance();

						measurementsTaken++;
					} else{
						// ERROR???
					}
				}



			}
		}

		if(measurementsTaken > 60){
			measurementsTaken = 0;	// If some error appears and the function is stuck, zero the counter
		}

	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
