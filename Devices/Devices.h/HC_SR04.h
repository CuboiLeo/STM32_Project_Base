/**
 * @file HC_SR04.h
 * @author Leo Liu
 * @brief get distance from the ultrasonic sensor
 * @version 1.0
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __HC_SR04_H
#define __HC_SR04_H
 
#include <stdio.h>
#include <stdint.h>
#include <tim.h>
#include "User_Defined_Math.h"
#include "Kalman_Filter.h"

#define HC_SR04_RELOAD_VALUE 100000

#define HC_SR04_Func_GroundInit       		\
    {                                 		\
				&HC_SR04_Init,										\
						&HC_SR04_Start,								\
    }

typedef struct
{
	float Distance_Raw;
	float Distance_KF;
	uint8_t Loop_Count;
	uint32_t Rising_Time;
	uint32_t Falling_Time;
	uint8_t Capture_State;
	uint32_t Calculated_Time;
}HC_SR04_t;

typedef struct
{
	void (*HC_SR04_Init)(void);
	void (*HC_SR04_Start)(void);
}HC_SR04_Func_t;

extern HC_SR04_t HC_SR04;
extern HC_SR04_Func_t HC_SR04_Func;

#endif
