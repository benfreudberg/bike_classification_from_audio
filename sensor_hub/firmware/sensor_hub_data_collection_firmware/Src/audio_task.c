#include "main.h"
#include "dfsdm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "audio_task.h"
#include "wav_header.h"
#include "freertos_vars.h"
#include "fatfs.h"
#include "timestamp.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define RECORDING_LENGTH_MS (8000) //must be multiple of (AUDIO_BUF_HALF_LEN/AUDIO_SAMPLE_RATE*1000) -> 20ms
#define FILE_QUEUE_LENGTH   (16) //16 comes from emptyBuf and fullBuf queue size

volatile int32_t raw_rec_buf[2][AUDIO_BUF_HALF_LEN];
static float processed_rec_buf[FILE_QUEUE_LENGTH][AUDIO_BUF_HALF_LEN];
static int buf_full_count = 0;
static FSIZE_t file_write_index = 0;

void StartAudioFileTask(void *argument) {
  float * processed_rec_buf_part;
  WavHeader header;
  uint32_t byteswritten;
  FRESULT res;

  osSemaphoreAcquire(file_system_readyHandle, osWaitForever);

  /* write header data into temporary wav file */
  WavHeader_Create(&header, RECORDING_LENGTH_MS);

  osMutexAcquire(fileMutexHandle, osWaitForever);
  res = f_open(&SDFile, "temp_audio_file.wav", FA_CREATE_ALWAYS | FA_WRITE);
  if (res) {
    printf("audio file open failed with result: %d\n", res);
    osThreadExit();
  }

  res = f_write(&SDFile, (const void*)&header, sizeof(WavHeader), (void *)&byteswritten);
  if (res) {
    printf("audio file header write failed with result: %d\n", res);
    osThreadExit();
  }
  file_write_index += byteswritten;

  f_close(&SDFile);
  osMutexRelease(fileMutexHandle);

  /* write streaming audio data into temporary wav file */
  while (1) {
    //could make this faster by getting all elements from the queue if there is more than one available and doing one (or two) big write operations
    osMessageQueueGet(audio_file_full_bufHandle, (void*)&processed_rec_buf_part, NULL, osWaitForever);
    if (processed_rec_buf_part == NULL) {
      //NULL is a marker to indicate that we are done streaming
      break;
    }

    osMutexAcquire(fileMutexHandle, osWaitForever);
    f_open(&SDFile, "temp_audio_file.wav", FA_WRITE);
    f_lseek(&SDFile, file_write_index);
    f_write(&SDFile, (const void*)processed_rec_buf_part, sizeof(float)*AUDIO_BUF_HALF_LEN, (void *)&byteswritten);
    f_close(&SDFile);
    file_write_index += byteswritten;
    osMutexRelease(fileMutexHandle);

    osMessageQueuePut(audio_file_empty_bufHandle, (void*)&processed_rec_buf_part, osPriorityNormal, osWaitForever);
  }

  /* copy temporary wav file to a new file with the current time as the name */
  char file_name[50]; //need 25 bytes
  TimeStamp_GetTimeStampString(file_name);
  strcat(file_name, ".wav");

  printf("copying file...\n");
  osMutexAcquire(fileMutexHandle, osWaitForever);
  //processed_rec_buf is no longer needed so we can reuse it here
  int copy_result = CopyFile("temp_audio_file.wav", file_name, (BYTE*)processed_rec_buf, sizeof(processed_rec_buf));
  osMutexRelease(fileMutexHandle);

  printf("copy_result: %d\n", copy_result);

  osSemaphoreRelease(task_finishedHandle);
  osThreadExit();
}

void StartAudioBufTask(void *argument) {
  volatile int32_t * raw_rec_buf_part;
  float * processed_rec_buf_part;
  bool finished = false;
  uint32_t available_bufs_low_point = 9999;

  //initialize audio_file_empty_buf queue
  for (int i = 0; i < FILE_QUEUE_LENGTH; i++) {
    processed_rec_buf_part = &processed_rec_buf[i][0];
    osMessageQueuePut(audio_file_empty_bufHandle, (void*)&processed_rec_buf_part, osPriorityNormal, 0);
  }

  while (1) {
    //get address of buffer that has fresh raw data ready from DFSDM DMA interrupt
    osMessageQueueGet(audioBufferReadyQueueHandle, (void*)&raw_rec_buf_part, NULL, osWaitForever);

    //stop streaming after the specified time
    if (buf_full_count >= RECORDING_LENGTH_MS * AUDIO_SAMPLE_RATE / AUDIO_BUF_HALF_LEN / 1000) {
      //stop the microphone PDM clocking and data collection
      HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0);
      printf("finished recording audio\n");
      finished = true;
    }

    /* process raw data into floats and put it in buffer for file task to save to SD card */
    uint32_t available_bufs = osMessageQueueGetCount(audio_file_empty_bufHandle);
    if (available_bufs < available_bufs_low_point) {
      available_bufs_low_point = available_bufs;
    }
    osStatus_t buf_queue_status = osMessageQueueGet(audio_file_empty_bufHandle,
                                                          (void*)&processed_rec_buf_part, NULL, 0);
    if (buf_queue_status != osOK) {
      printf("ERROR: issue getting empty buffer pointer\n");
      osThreadExit();
    }
    for (int i = 0; i < AUDIO_BUF_HALF_LEN; i++) {
      // full scale range with Sinc3 and FOSR set to 68 is +/- 68^3, approximately 2^18.26
      processed_rec_buf_part[i] = ((float)(raw_rec_buf_part[i]>>8)) / (2<<18); //only upper 24 bits are data
    }
    buf_queue_status = osMessageQueuePut(audio_file_full_bufHandle, (void*)&processed_rec_buf_part, osPriorityNormal, 0);
    if (buf_queue_status != osOK) {
      printf("ERROR: issue sending full buffer pointer\n");
      osThreadExit();
    }

    if (finished) {
      printf("available bufs low point: %lu\n", available_bufs_low_point);
      processed_rec_buf_part = NULL; //NULL is a marker to indicate that we are done streaming
      osMessageQueuePut(audio_file_full_bufHandle, (void*)&processed_rec_buf_part, osPriorityNormal, 0);
      osThreadExit();
    }
  }
}


void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter){
  volatile int32_t * const raw_rec_buf_part = &raw_rec_buf[0][0];
  buf_full_count++;
  osMessageQueuePut(audioBufferReadyQueueHandle, (void*)&raw_rec_buf_part, osPriorityNormal, 0);
  //add warning of missed data if queuePut finds full queue?
}

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  volatile int32_t * const raw_rec_buf_part = &raw_rec_buf[1][0];
  buf_full_count++;
  osMessageQueuePut(audioBufferReadyQueueHandle, (void*)&raw_rec_buf_part, osPriorityNormal, 0);
  //add warning of missed data if queuePut finds full queue?
}
