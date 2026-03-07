///*
// * HCSR04_Driver.c
// *
// *  Created on: Feb 21, 2026
// *      Author: 48794
// */
//#include "HCSR04_Driver.h"
//
//static TIM_HandleTypeDef *hcsr04_tim;
//static GPIO_TypeDef *hcsr04_trig_port;
//static uint16_t hcsr04_trig_pin;
//
//static uint32_t distance = 0;
//
//volatile uint32_t ticks1, ticks2;
//volatile uint8_t timeCaptured = 0;
//
//void HCSR04_Init(TIM_HandleTypeDef *htim,
//                 GPIO_TypeDef *trig_port,
//                 uint16_t trig_pin)
//{
//    hcsr04_tim = htim;
//    hcsr04_trig_port = trig_port;
//    hcsr04_trig_pin = trig_pin;
//}
//
//void HCSR04_startMeasurement(){
//	HAL_GPIO_WritePin(hcsr04_trig_port, hcsr04_trig_pin, GPIO_PIN_SET);
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(hcsr04_trig_port, hcsr04_trig_pin, GPIO_PIN_RESET);
//}
//
//void HCSR04_IC_Callback(TIM_HandleTypeDef *htim){
//    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
//
//        if (timeCaptured == 0){	// First echo pin slope?
//        	ticks1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);	// capture sensor echo pin Rising Edge
//
//        	timeCaptured = 1;	// next time capture second echo pin slope
//            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);	// Set capturing Falling Edge
//        } else{
//
//        	ticks2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // capture sensor echo pin Falling Edge
//
//        	timeCaptured = 0;	// next time capture first echo pin slope
//            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);	// capture sensor echo pin Rising Edge
//        }
//	}
//}
//
//uint32_t HCSR04_receiveDistance(){
//    if (ticks2 >= ticks1)
//        distance = ticks2 - ticks1;
//    else
//        distance = (0xFFFF - ticks1) + ticks2;	// Timer register overflow compensation
//
//    return (distance / (2 * CLK_FREQ))/340 ;   // return calculated distance for CLK_FREQ configuration at 15°C
//}
//
