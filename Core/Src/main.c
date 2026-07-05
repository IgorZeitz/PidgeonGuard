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
#include "app_fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TIM3_MAX_VALUE  0x0000FFFFu

#define SMOTOR124_PORT GPIOA
#define SMOTOR3_PORT GPIOB
#define SMOTOR_INI1_PIN GPIO_PIN_11
#define SMOTOR_INI2_PIN GPIO_PIN_12
#define SMOTOR_INI3_PIN GPIO_PIN_7
#define SMOTOR_INI4_PIN GPIO_PIN_1

#define PUMP_PORT GPIOA
#define PUMP_PIN GPIO_PIN_14

#define HC_TRIG_PORT GPIOB
#define HC_TRIG_PIN GPIO_PIN_8

#define PWR_INIT_PORT GPIOC
#define PWR_INIT_PIN GPIO_PIN_14
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;


char TxBuffer[250];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

//void Flash_Unlock(void) {
//    // Unlock Flash memory
//    FLASH->KEYR = 0x45670123;
//    FLASH->KEYR = 0xCDEF89AB;
//    // Unlock Option Bytes
//    FLASH->OPTKEYR = 0x08192A3B;
//    FLASH->OPTKEYR = 0x4C5D6E7F;
//}

//void Boot0_Enable(void) {
//    // Clear the nBOOT_SEL bit to enable BOOT0 pin functionality
//    FLASH->OPTR &= ~FLASH_OPTR_nBOOT_SEL;
//    // Wait for any ongoing flash operation to complete
//    while(FLASH->SR & FLASH_SR_BSY1);
//    // Start the Option Byte programming
//    FLASH->CR |= FLASH_CR_OPTSTRT;
//    // Wait for the programming to complete
//    while(FLASH->SR & FLASH_SR_BSY1);
//    // Launch the option byte loading
//    FLASH->CR |= FLASH_CR_OBL_LAUNCH;
//}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint32_t l_hcrs04Ticks1, l_hcrs04Ticks2;
volatile uint8_t l_hcrs04TimeCaptured = 0;
volatile uint32_t distance_L;

uint32_t mappedSourrounding[60];


FATFS FatFs;
FIL Fil;
FRESULT FR_Status;
FATFS *FS_Ptr;
UINT RWC, WWC; // Read/Write Word Counter
DWORD FreeClusters;
uint32_t TotalSize, FreeSpace;
char RW_Buffer[200];

static void UART_Print(char* str)
{
    HAL_UART_Transmit(&huart2 , (uint8_t *) str, strlen(str), 100);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	//BOOT PIN
	// Check if the nBOOT_SEL bit is already cleared
	//HAL_Delay(8000);	// wait for 8s before changing configuration

	  //  if ((FLASH->OPTR & FLASH_OPTR_nBOOT_SEL) == 0)
	  //  {
	        // If already cleared, do nothing
	//        for (;;);
	 //   // Unlock flash memory and option bytes
	 //   Flash_Unlock();
	    // Enable BOOT0 pin functionality
	//    Boot0_Enable();
	    // We should never reach this point as the system will reset
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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  if (MX_FATFS_Init() != APP_OK) {
    Error_Handler();
  }
  MX_RTC_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  // Enable interrupts for input capture timers for both HC-SR04
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
    //HCSR04_Init(&htim3, GPIOC, GPIO_PIN_6);

    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    uint8_t message[64];
    //SD_Card_Test();
    //HCSR04_Init(&htim3, GPIOC, GPIO_PIN_6);
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0xBEEF){
    sTime.Hours = 21;
    sTime.Minutes = 37;
    sTime.Seconds = 0;

    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_APRIL;
    sDate.Date = 8;
    sDate.Year = 26;

    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0xBEEF);
  }


  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
   HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

   sprintf(RW_Buffer, "WKUP: %02d-%02d-20%02d %02d:%02d:%02d\r\n",
           sDate.Date,
           sDate.Month,
           sDate.Year,
           sTime.Hours,
           sTime.Minutes,
           sTime.Seconds);

   FR_Status = f_mount(&FatFs, "", 1);
       if (FR_Status != FR_OK)
       {
         sprintf(TxBuffer, "\r\nError! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
         UART_Print(TxBuffer);
       }

   //f_mount(&FatFs, "", 1);
       FR_Status = f_open(&Fil, "TextFileWrite.txt", FA_WRITE | FA_READ);
   if (FR_Status != FR_OK)
   {
     sprintf(TxBuffer, "\r\nError! While Opening File: (%i)\r\n", FR_Status);
     UART_Print(TxBuffer);
   }

   f_lseek(&Fil, f_size(&Fil));
   f_puts(RW_Buffer, &Fil);
   f_close(&Fil);
   //f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
  f_mount(NULL, "", 0);

 	calibrateDistance();

  while (1)
  {
	  // PWR TEST
	  	HAL_Delay(5000);

	  	HAL_GPIO_WritePin(PWR_INIT_PORT, PWR_INIT_PIN, GPIO_PIN_SET);
	  	sprintf((char*)message, "HIGH\n");
	  			HAL_UART_Transmit(&huart2, message, strlen((char*)message), 1000);

	  	HAL_Delay(10000);

	  	HAL_GPIO_WritePin(PWR_INIT_PORT, PWR_INIT_PIN, GPIO_PIN_RESET);
	  	sprintf((char*)message, "LOW\n");
	  			HAL_UART_Transmit(&huart2, message, strlen((char*)message), 1000);

	  			HAL_Delay(10000);
	  	//PWR END TEST

	  	//sprintf((char*)message, "MAP: %lu\n", mappedSourrounding[10]);
	  	//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);

	  	//calibrateDistance();
	  	lookForTarget();

	  	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
	  	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

		sprintf((char*)message, "Spij\n");
		HAL_UART_Transmit(&huart2, message, strlen((char*)message), 1000);
	  	HAL_PWR_EnterSTANDBYMode();
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB8 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
	  HAL_GPIO_WritePin(HC_TRIG_PORT, HC_TRIG_PIN, GPIO_PIN_SET);
	  HAL_Delay(10);
	  HAL_GPIO_WritePin(HC_TRIG_PORT, HC_TRIG_PIN, GPIO_PIN_RESET);
}

void calculateDistance(){
	if(l_hcrs04Ticks2 > l_hcrs04Ticks1){
		distance_L = l_hcrs04Ticks2 - l_hcrs04Ticks1;
	} else distance_L = TIM3_MAX_VALUE - (l_hcrs04Ticks1 - l_hcrs04Ticks2);

	uint32_t message[16];
  	//sprintf((char*)message, "DIST: %lu\n", distance_L);
  	//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
}


void controlSMotor(int32_t stepNumber, uint8_t direction){
	int32_t stepsPerformed = 0;
	if(direction == 0){
		while(stepNumber > stepsPerformed){	//	Turn Right
		switch (stepsPerformed % 4) {
		      case 0:  // 1010
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 1:  // 0110
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 2:  //0101
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_SET);
		      break;
		      case 3:  //1001
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_SET);
		      break;
		    }
		stepsPerformed++;
		HAL_Delay(5);
		}
	} else{
		while(stepNumber > stepsPerformed){	//	Turn Left
			switch ( stepNumber % 4) {
		      case 0:  // 1010
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 1:  // 0110
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_RESET);
		      break;
		      case 2:  //0101
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_SET);
		      break;
		      case 3:  //1001
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI1_PIN, GPIO_PIN_SET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI2_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR3_PORT, SMOTOR_INI3_PIN, GPIO_PIN_RESET);
		        HAL_GPIO_WritePin(SMOTOR124_PORT, SMOTOR_INI4_PIN, GPIO_PIN_SET);
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
	HAL_UART_Transmit(&huart2, message, strlen((char*)message), 1000);

}


void lookForTarget(){
	uint8_t oneStep = 12;
	uint8_t numberOfMeasurements = 60;
	uint8_t measurementsTaken = 0;

	uint8_t logger = 0;

	for(uint8_t i = 0; i<4; i++ ){ // Repeat full scanning 4 times

		for(; measurementsTaken < numberOfMeasurements; ){
			uint32_t message[16];
				//sprintf((char*)message, "STRZELAĆ!!!");


				startDistanceMeasurement();
				calculateDistance();

				uint32_t supposedValue = mappedSourrounding[measurementsTaken];

				controlSMotor(oneStep, 0);
				measurementsTaken++;

				logger = 0;

				while(distance_L + 1000 < supposedValue){

					// Save to logs that shooting happened
					if(logger == 0){
						HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
						  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

						  sprintf(RW_Buffer, "SHOOT: %02d-%02d-20%02d %02d:%02d:%02d\r\n",
						          sDate.Date,
						          sDate.Month,
						          sDate.Year,
						          sTime.Hours,
						          sTime.Minutes,
						          sTime.Seconds);

						  FR_Status = f_mount(&FatFs, "", 1);
						      if (FR_Status != FR_OK)
						      {
						        sprintf(TxBuffer, "\r\nError! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
						        UART_Print(TxBuffer);
						      }


						      FR_Status = f_open(&Fil, "TextFileWrite.txt", FA_WRITE | FA_READ);
						  if (FR_Status != FR_OK)
						  {
						    sprintf(TxBuffer, "\r\nError! While Opening File: (%i)\r\n", FR_Status);
						    UART_Print(TxBuffer);
						  }


						  f_lseek(&Fil, f_size(&Fil));
						  f_puts(RW_Buffer, &Fil);
						  f_close(&Fil);
						  //f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
						 f_mount(NULL, "", 0);

						 logger = 1;
					}

					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
					HAL_Delay(50);

					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					uint32_t lastDistance = distance_L;

					startDistanceMeasurement();
					calculateDistance();

					if(lastDistance - 70 > distance_L){

						lastDistance = distance_L;

						controlSMotor(oneStep, 0);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
						HAL_Delay(50);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						startDistanceMeasurement();
						calculateDistance();

						measurementsTaken++;

					} else if(lastDistance < distance_L - 70){

						lastDistance = distance_L;

						controlSMotor(oneStep, 1);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
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
					//sprintf((char*)message, "STRZELAĆ!!!");


				startDistanceMeasurement();
				calculateDistance();

				uint32_t supposedValue = mappedSourrounding[measurementsTaken];

				controlSMotor(oneStep, 1);
				measurementsTaken--;

				logger = 0;

				while(distance_L + 1000 < supposedValue){

					// Save to logs that shooting happened
					if(logger == 0){
						HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
						  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

						  sprintf(RW_Buffer, "SHOOT: %02d-%02d-20%02d %02d:%02d:%02d\r\n",
						          sDate.Date,
						          sDate.Month,
						          sDate.Year,
						          sTime.Hours,
						          sTime.Minutes,
						          sTime.Seconds);

						  FR_Status = f_mount(&FatFs, "", 1);
						      if (FR_Status != FR_OK)
						      {
						        sprintf(TxBuffer, "\r\nError! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
						        UART_Print(TxBuffer);
						      }


						      FR_Status = f_open(&Fil, "TextFileWrite.txt", FA_WRITE | FA_READ);
						  if (FR_Status != FR_OK)
						  {
						    sprintf(TxBuffer, "\r\nError! While Opening File: (%i)\r\n", FR_Status);
						    UART_Print(TxBuffer);
						  }


						  f_lseek(&Fil, f_size(&Fil));
						  f_puts(RW_Buffer, &Fil);
						  f_close(&Fil);
						  //f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
						 f_mount(NULL, "", 0);

						 logger = 1;
					}

					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
					HAL_Delay(50);
					HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

					uint32_t lastDistance = distance_L;

					startDistanceMeasurement();
					calculateDistance();


					if(lastDistance - 70 > distance_L){

						lastDistance = distance_L;

						controlSMotor(oneStep, 1);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
						HAL_Delay(50);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						startDistanceMeasurement();
						calculateDistance();

						measurementsTaken--;

					} else if(lastDistance < distance_L - 70){

						lastDistance = distance_L;

						controlSMotor(oneStep, 0);
						HAL_GPIO_TogglePin(PUMP_PORT, PUMP_PIN);

						//HAL_UART_Transmit(&huart1, message, strlen((char*)message), 1000);
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
