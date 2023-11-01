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
 
#include "Kalman_Filter.h"

float First_Order_Kalman_Filter(Kalman_Filter_t *KF, float Measurement);

Kalman_Filter_t HC_SR04_KF = HC_SR04_KF_Init;
#undef HC_SR04_KF_Init

Kalman_Filter_Func_t Kalman_Filter_Func = Kalman_Filter_Func_GroundInit;
#undef Kalman_Filter_Func_GroundInit

float First_Order_Kalman_Filter(Kalman_Filter_t *KF, float Measurement)
{
	KF->Current_P = KF->Prev_P + KF->Q;
	KF->K = KF->Current_P / (KF->Current_P + KF->R);
	KF->Output = KF->Output + KF->K*(Measurement - KF->Output);
	KF->Prev_P = (1 - KF->K)*KF->Current_P;
	
	return KF->Output;
}
