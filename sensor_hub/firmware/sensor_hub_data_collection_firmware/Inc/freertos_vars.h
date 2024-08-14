/*
 * freertos_vars.h
 *
 *  Created on: Aug 5, 2024
 *      Author: Fongberg
 */

#ifndef FREERTOS_VARS_H_
#define FREERTOS_VARS_H_

extern osThreadId_t audio_buf_taskHandle;
extern osThreadId_t audio_file_taskHandle;
extern osMutexId_t fileMutexHandle;
extern osMessageQueueId_t audioBufferReadyQueueHandle;
extern osSemaphoreId_t audio_file_readyHandle;
extern osSemaphoreId_t audio_buf_finishedHandle;

#endif /* FREERTOS_VARS_H_ */
