/**
 * @file Kalman Filter.c
 * @author Leo Liu
 * @brief Kalman Filter Algorithm Library
 * @version 1.0
 * @date 2023-09-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __KALMAN_FILTER_H
#define __KALMAN_FILTER_H

#include <stdio.h>
#include <stdint.h>

#define Kalman_Filter_Func_GroundInit     \
    {                                 		\
				&First_Order_Kalman_Filter,				\
    }

typedef struct
{
	float Prev_P;
	float Current_P;
	float Output;
	float K;
	float Q;
	float R;
}Kalman_Filter_t;

typedef struct
{
	float (*First_Order_Kalman_Filter)(Kalman_Filter_t *KF, float Measurement);
}Kalman_Filter_Func_t;

#define HC_SR04_KF_Init				\
		{													\
				1.0f,									\
					0,									\
					0,									\
					0,									\
					0.00001f,								\
					0.01f,							\
		}

extern Kalman_Filter_t HC_SR04_KF;
extern Kalman_Filter_Func_t Kalman_Filter_Func;

#endif
