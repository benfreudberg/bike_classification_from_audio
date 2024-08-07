/*
 * freertos_vars.h
 *
 *  Created on: Aug 5, 2024
 *      Author: Fongberg
 */

#ifndef FREERTOS_VARS_H_
#define FREERTOS_VARS_H_

extern osThreadId_t audio_buf_taskHandle;
extern osThreadId_t audio_buf_task2Handle;
extern osThreadId_t audioFileTaskHandle;
extern osMutexId_t fileMutexHandle;
extern osSemaphoreId_t audio_buf_1st_data_readyHandle;
extern osSemaphoreId_t audio_buf_2nd_data_readyHandle;

#endif /* FREERTOS_VARS_H_ */
