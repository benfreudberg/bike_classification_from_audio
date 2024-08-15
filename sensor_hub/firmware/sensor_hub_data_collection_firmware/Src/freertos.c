/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "audio_task.h"
#include "fatfs.h"
#include <stdio.h>
#include <string.h>
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
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for leds_task */
osThreadId_t leds_taskHandle;
const osThreadAttr_t leds_task_attributes = {
  .name = "leds_task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for audio_buf_task */
osThreadId_t audio_buf_taskHandle;
const osThreadAttr_t audio_buf_task_attributes = {
  .name = "audio_buf_task",
  .stack_size = 5000 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for audio_file_task */
osThreadId_t audio_file_taskHandle;
const osThreadAttr_t audio_file_task_attributes = {
  .name = "audio_file_task",
  .stack_size = 5000 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for audioBufferReadyQueue */
osMessageQueueId_t audioBufferReadyQueueHandle;
const osMessageQueueAttr_t audioBufferReadyQueue_attributes = {
  .name = "audioBufferReadyQueue"
};
/* Definitions for audio_file_empty_buf */
osMessageQueueId_t audio_file_empty_bufHandle;
const osMessageQueueAttr_t audio_file_empty_buf_attributes = {
  .name = "audio_file_empty_buf"
};
/* Definitions for audio_file_full_buf */
osMessageQueueId_t audio_file_full_bufHandle;
const osMessageQueueAttr_t audio_file_full_buf_attributes = {
  .name = "audio_file_full_buf"
};
/* Definitions for fileMutex */
osMutexId_t fileMutexHandle;
const osMutexAttr_t fileMutex_attributes = {
  .name = "fileMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void StartLedsTask(void *argument);
extern void StartAudioBufTask(void *argument);
extern void StartAudioFileTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
  printf("stack overflow detected on task: %s\n", pcTaskName);
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of fileMutex */
  fileMutexHandle = osMutexNew(&fileMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of audioBufferReadyQueue */
  audioBufferReadyQueueHandle = osMessageQueueNew (1, sizeof(int32_t*), &audioBufferReadyQueue_attributes);

  /* creation of audio_file_empty_buf */
  audio_file_empty_bufHandle = osMessageQueueNew (32, sizeof(float *), &audio_file_empty_buf_attributes);

  /* creation of audio_file_full_buf */
  audio_file_full_bufHandle = osMessageQueueNew (32, sizeof(float *), &audio_file_full_buf_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of leds_task */
  leds_taskHandle = osThreadNew(StartLedsTask, NULL, &leds_task_attributes);

  /* creation of audio_buf_task */
  audio_buf_taskHandle = osThreadNew(StartAudioBufTask, NULL, &audio_buf_task_attributes);

  /* creation of audio_file_task */
  audio_file_taskHandle = osThreadNew(StartAudioFileTask, NULL, &audio_file_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osThreadSuspend(osThreadGetId());
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

