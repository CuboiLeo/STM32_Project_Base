/**
 * @file HC_SR04.c
 * @author Leo Liu
 * @brief get distance from the ultrasonic sensor
 * @version 1.0
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "HC_SR04.h"

void HC_SR04_Init(void);
void HC_SR04_Start(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

HC_SR04_t HC_SR04;
HC_SR04_Func_t HC_SR04_Func = HC_SR04_Func_GroundInit;
#undef HC_SR04_Func_GroundInit

void HC_SR04_Init(void)
{
	//HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_4);
}

void HC_SR04_Start(void)
{
	HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port,HC_SR04_Trig_Pin,GPIO_PIN_SET);
	DELAY_US(20);
	HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port,HC_SR04_Trig_Pin,GPIO_PIN_RESET);
	HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_4);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(TIM5 == htim->Instance)
	{
		if(HC_SR04.Capture_State == 0)
		{
			HC_SR04.Rising_Time = HAL_TIM_ReadCapturedValue(&htim5,TIM_CHANNEL_4);
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_FALLING);
			HC_SR04.Capture_State = 1;
		}
		else if(HC_SR04.Capture_State == 1)
		{
			HC_SR04.Falling_Time = HAL_TIM_ReadCapturedValue(&htim5,TIM_CHANNEL_4);
			__HAL_TIM_SET_COUNTER(htim,0);
			
			if(HC_SR04.Rising_Time < HC_SR04.Falling_Time)
				HC_SR04.Calculated_Time = HC_SR04.Falling_Time - HC_SR04.Rising_Time;
			else if(HC_SR04.Rising_Time > HC_SR04.Falling_Time)
				HC_SR04.Calculated_Time = (HC_SR04_RELOAD_VALUE - HC_SR04.Rising_Time) + HC_SR04.Falling_Time;
			
			if(((HC_SR04.Calculated_Time / 1000000.0f) * 340.0f / 2.0f * 100.0f) < 500.0f)
				HC_SR04.Distance_Raw = (HC_SR04.Calculated_Time / 1000000.0f) * 340.0f / 2.0f * 100.0f;
			HC_SR04.Distance_KF = Kalman_Filter_Func.First_Order_Kalman_Filter(&HC_SR04_KF,HC_SR04.Distance_Raw);
			
			HC_SR04.Capture_State = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_RISING);
			HAL_TIM_IC_Stop_IT(&htim5,TIM_CHANNEL_4);
		}
	}
}

