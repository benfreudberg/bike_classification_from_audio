/*
 * freertos_vars.h
 *
 *  Created on: Aug 5, 2024
 *      Author: Fongberg
 */

#ifndef FREERTOS_VARS_H_
#define FREERTOS_VARS_H_

#include "cmsis_os.h"

extern osThreadId_t audio_file_taskHandle;
extern osThreadId_t cam_taskHandle;
extern osMutexId_t fileMutexHandle;
extern osMessageQueueId_t audioBufferReadyQueueHandle;
extern osMessageQueueId_t audio_file_empty_bufHandle;
extern osMessageQueueId_t audio_file_full_bufHandle;
extern osSemaphoreId_t file_system_readyHandle;
extern osSemaphoreId_t task_finishedHandle;
extern osSemaphoreId_t spi1_semHandle;
extern osSemaphoreId_t spi2_semHandle;
extern osSemaphoreId_t i2c2_semHandle;

#endif /* FREERTOS_VARS_H_ */
