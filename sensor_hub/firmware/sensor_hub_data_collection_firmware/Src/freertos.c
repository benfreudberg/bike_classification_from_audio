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
#include "fatfs.h"
#include "spi.h"
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
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow2,
};
/* Definitions for audio_buf_task */
osThreadId_t audio_buf_taskHandle;
const osThreadAttr_t audio_buf_task_attributes = {
  .name = "audio_buf_task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for audio_file_task */
osThreadId_t audio_file_taskHandle;
const osThreadAttr_t audio_file_task_attributes = {
  .name = "audio_file_task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for mag_task */
osThreadId_t mag_taskHandle;
const osThreadAttr_t mag_task_attributes = {
  .name = "mag_task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for cam_task */
osThreadId_t cam_taskHandle;
const osThreadAttr_t cam_task_attributes = {
  .name = "cam_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal6,
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
/* Definitions for spi1_sem */
osSemaphoreId_t spi1_semHandle;
const osSemaphoreAttr_t spi1_sem_attributes = {
  .name = "spi1_sem"
};
/* Definitions for spi2_sem */
osSemaphoreId_t spi2_semHandle;
const osSemaphoreAttr_t spi2_sem_attributes = {
  .name = "spi2_sem"
};
/* Definitions for i2c2_sem */
osSemaphoreId_t i2c2_semHandle;
const osSemaphoreAttr_t i2c2_sem_attributes = {
  .name = "i2c2_sem"
};
/* Definitions for file_system_ready */
osSemaphoreId_t file_system_readyHandle;
const osSemaphoreAttr_t file_system_ready_attributes = {
  .name = "file_system_ready"
};
/* Definitions for task_finished */
osSemaphoreId_t task_finishedHandle;
const osSemaphoreAttr_t task_finished_attributes = {
  .name = "task_finished"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void StartAudioBufTask(void *argument);
extern void StartAudioFileTask(void *argument);
extern void StartMagTask(void *argument);
extern void StartCamTask(void *argument);

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

static void SystemShutdown(void) {
  osKernelLock();
  __disable_irq();
  //set INT_OUT pin to inform Particle that power to Sensor Hub can be shut off
  HAL_GPIO_WritePin(INT_OUT_GPIO_Port, INT_OUT_Pin, GPIO_PIN_SET);
  //other power modes could save more power, but then debugger can't connect
  HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
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

  /* Create the semaphores(s) */
  /* creation of spi1_sem */
  spi1_semHandle = osSemaphoreNew(1, 0, &spi1_sem_attributes);

  /* creation of spi2_sem */
  spi2_semHandle = osSemaphoreNew(1, 0, &spi2_sem_attributes);

  /* creation of i2c2_sem */
  i2c2_semHandle = osSemaphoreNew(1, 0, &i2c2_sem_attributes);

  /* creation of file_system_ready */
  file_system_readyHandle = osSemaphoreNew(3, 0, &file_system_ready_attributes);

  /* creation of task_finished */
  task_finishedHandle = osSemaphoreNew(3, 0, &task_finished_attributes);

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

  /* creation of audio_buf_task */
  audio_buf_taskHandle = osThreadNew(StartAudioBufTask, NULL, &audio_buf_task_attributes);

  /* creation of audio_file_task */
  audio_file_taskHandle = osThreadNew(StartAudioFileTask, NULL, &audio_file_task_attributes);

  /* creation of mag_task */
  mag_taskHandle = osThreadNew(StartMagTask, NULL, &mag_task_attributes);

  /* creation of cam_task */
  cam_taskHandle = osThreadNew(StartCamTask, NULL, &cam_task_attributes);

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
  //mount sd card
  FRESULT res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
  if (res) {
    printf("sd mount failed with result: %d\n", res);
    osThreadExit();
  }

#if 0 //reformat sd card
  uint8_t rtext[_MAX_SS];
  res = f_mkfs((TCHAR const*)SDPath, FM_FAT32, 0, rtext, sizeof(rtext));
  if (res) {
    printf("sd reformat failed with result: %d\n", res);
  } else {
    printf("sd card reformated\n");
  }
  osThreadExit();
#endif

  DWORD nclst;
  FATFS *SDFatFS_ptr = &SDFatFS;
  float free_space_MB;
  f_getfree((TCHAR const*)SDPath, &nclst, &SDFatFS_ptr);
  free_space_MB = (float)nclst * SDFatFS.csize * 512 / (1 << 20);
  printf("freespace: %.2fMB\n", free_space_MB);
  if (free_space_MB < 5) {
    //turn on red LED to indicate SD card error
    HAL_GPIO_WritePin(GPIOE, LED3_Pin, GPIO_PIN_SET);
    printf("ERROR: sd card has less than 5 MB free\n");
    osDelay(25);
    SystemShutdown();
  }

  while(osSemaphoreRelease(file_system_readyHandle) == osOK); //inform all threads that the file system is ready

  //flash green led to indicate no issues with SD card
  HAL_GPIO_WritePin(GPIOE, LED1_Pin, GPIO_PIN_SET);
  osDelay(50);
  HAL_GPIO_WritePin(GPIOE, LED1_Pin, GPIO_PIN_RESET);

  //wait for all data collection threads to indicate they are done
  for (int i = 0; i < 3; i++) { //audio, mag, and camera threads all indicate they are finished
    osSemaphoreAcquire(task_finishedHandle, osWaitForever);
  }

  //unmount sd card
  f_mount(&SDFatFS, (TCHAR const*)NULL, 0);

  //turn on orange LED to indicate processing finished
  HAL_GPIO_WritePin(GPIOE, LED2_Pin, GPIO_PIN_SET);
  printf("all processes finished after %lu ticks\n", HAL_GetTick());
  SystemShutdown();
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

