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
  .stack_size = 5000 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ledsTask */
osThreadId_t ledsTaskHandle;
const osThreadAttr_t ledsTask_attributes = {
  .name = "ledsTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void StartLedsTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ledsTask */
  ledsTaskHandle = osThreadNew(StartLedsTask, NULL, &ledsTask_attributes);

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
  uint32_t scheduling_kernel_tick_count = 0;
  /* Infinite loop */
  for(;;)
  {
    FRESULT res; /* FatFs function common result code */
    uint32_t byteswritten, bytesread; /* File write/read counts */
    uint8_t wtext[] = "STM32 FATFS works great!"; /* File write buffer */
    uint8_t rtext[_MAX_SS];/* File read buffer */
    char file_name[50];

    uint32_t kernel_tick_count = osKernelGetTickCount();
    sprintf(file_name, "kernal_tick_count_is_%lu.txt", kernel_tick_count);

    res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
    printf("res: %d\n", res);
    if(res != FR_OK)
     {
         Error_Handler();
     }
     else
     {
//       res = f_mkfs((TCHAR const*)SDPath, FM_FAT32, 0, rtext, sizeof(rtext));
//       printf("res: %d\n", res);
       if(res != FR_OK)
       {
           Error_Handler();
       }
       else
       {
         //Open file for writing (Create)
         res = f_open(&SDFile, file_name, FA_CREATE_ALWAYS | FA_WRITE);
         printf("res: %d\n", res);
         if(res != FR_OK)
         {
             Error_Handler();
         }
         else
         {
           //Write to the text file
           res = f_write(&SDFile, wtext, strlen((char *)wtext), (void *)&byteswritten);
           printf("res: %d\n", res);
           if((byteswritten == 0) || (res != FR_OK))
           {
             Error_Handler();
           }
           else
           {
             f_close(&SDFile);
           }
         }
       }
     }
    f_mount(&SDFatFS, (TCHAR const*)NULL, 0);

    scheduling_kernel_tick_count += 1000;
    osDelayUntil(scheduling_kernel_tick_count);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

