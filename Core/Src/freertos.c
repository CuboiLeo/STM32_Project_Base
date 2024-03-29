/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Board_A_IMU.h"
#include "IMU_Temp_Control.h"
#include "DR16_Remote.h"
#include "Fusion.h"
#include "Motors.h"
#include "Robot_Control.h"
#include "HC_SR04.h"
#include "Serial.h"
#include "Buzzer.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Task_IMUHandle;
osThreadId Task_InitHandle;
osThreadId Task_Robot_CtrlHandle;
osThreadId Task_SerialHandle;
osThreadId Task_WarningHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartIMUTask(void const * argument);
void General_Init(void const * argument);
void Robot_Control(void const * argument);
void Serial_Send(void const * argument);
void Sound_Warning(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task_IMU */
  osThreadDef(Task_IMU, StartIMUTask, osPriorityNormal, 0, 384);
  Task_IMUHandle = osThreadCreate(osThread(Task_IMU), NULL);

  /* definition and creation of Task_Init */
  osThreadDef(Task_Init, General_Init, osPriorityHigh, 0, 128);
  Task_InitHandle = osThreadCreate(osThread(Task_Init), NULL);

  /* definition and creation of Task_Robot_Ctrl */
  osThreadDef(Task_Robot_Ctrl, Robot_Control, osPriorityRealtime, 0, 640);
  Task_Robot_CtrlHandle = osThreadCreate(osThread(Task_Robot_Ctrl), NULL);

  /* definition and creation of Task_Serial */
  osThreadDef(Task_Serial, Serial_Send, osPriorityHigh, 0, 128);
  Task_SerialHandle = osThreadCreate(osThread(Task_Serial), NULL);

  /* definition and creation of Task_Warning */
  osThreadDef(Task_Warning, Sound_Warning, osPriorityNormal, 0, 128);
  Task_WarningHandle = osThreadCreate(osThread(Task_Warning), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartIMUTask */
/**
  * @brief  Function implementing the Task_IMU thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartIMUTask */
void StartIMUTask(void const * argument)
{
  /* USER CODE BEGIN StartIMUTask */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  const TickType_t TimeIncrement = pdMS_TO_TICKS(2);
  /* Infinite loop */
  for(;;)
  {
		Board_A_IMU_Func.Board_A_IMU_Read_Data(&Board_A_IMU);
		Board_A_IMU_Func.Board_A_IMU_Calc_Angle(&Board_A_IMU);
		Board_A_IMU_Func.Board_A_IMU_Calibrate(&Board_A_IMU);
		IMU_Temp_Control_Func.Board_A_IMU_Temp_Control();
		HC_SR04_Func.HC_SR04_Start();
		
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END StartIMUTask */
}

/* USER CODE BEGIN Header_General_Init */
/**
* @brief Function implementing the Task_Init thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_General_Init */
void General_Init(void const * argument)
{
  /* USER CODE BEGIN General_Init */
	IMU_Temp_Control_Func.Board_A_IMU_Temp_Control_Init();
	Board_A_IMU_Func.Board_A_IMU_Init();
	FusionAhrsInitialise(&Board_A_IMU_AHRS);
	DR16_Func.DR16_USART_Receive_DMA(&huart1);
	Motors_Func.Motors_Init();
	HC_SR04_Func.HC_SR04_Init();
	Buzzer_Func.Buzzer_Init();
	vTaskDelete(NULL);
  /* Infinite loop */
  /* USER CODE END General_Init */
}

/* USER CODE BEGIN Header_Robot_Control */
/**
* @brief Function implementing the Task_Robot_Ctrl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Robot_Control */
void Robot_Control(void const * argument)
{
  /* USER CODE BEGIN Robot_Control */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  const TickType_t TimeIncrement = pdMS_TO_TICKS(2);
  /* Infinite loop */
  for(;;)
  {
		Robot_Func.Robot_Get_Data();
		Robot_Func.Robot_In_Control();
		HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
		osDelay(1000);
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Robot_Control */
}

/* USER CODE BEGIN Header_Serial_Send */
/**
* @brief Function implementing the Task_Serial thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Serial_Send */
void Serial_Send(void const * argument)
{
  /* USER CODE BEGIN Serial_Send */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  const TickType_t TimeIncrement = pdMS_TO_TICKS(10);
  /* Infinite loop */
  for(;;)
  {
    printf("/*%f,%f*/\n",HC_SR04.Distance_Raw,HC_SR04.Distance_KF);
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Serial_Send */
}

/* USER CODE BEGIN Header_Sound_Warning */
/**
* @brief Function implementing the Task_Warning thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Sound_Warning */
void Sound_Warning(void const * argument)
{
  /* USER CODE BEGIN Sound_Warning */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  const TickType_t TimeIncrement = pdMS_TO_TICKS(10);
  /* Infinite loop */
  for(;;)
  {
		if(HC_SR04.Distance_KF < 80.0f)
			Buzzer_Func.Buzzer_Warning();
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Sound_Warning */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
