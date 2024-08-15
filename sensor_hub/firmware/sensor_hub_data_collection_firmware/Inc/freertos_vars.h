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
extern osMessageQueueId_t audio_file_empty_bufHandle;
extern osMessageQueueId_t audio_file_full_bufHandle;

#endif /* FREERTOS_VARS_H_ */
