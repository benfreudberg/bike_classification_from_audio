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

#define RECORDING_LENGTH_MS (10000)

volatile int32_t raw_rec_buf[2][AUDIO_BUF_LEN/2];
static float processed_rec_buf[AUDIO_BUF_LEN/2];
static int buf_full_count = 0;
static FSIZE_t file_write_index = 0;

void StartAudioFileTask(void *argument) {
  WavHeader header;
  uint32_t byteswritten;
  FRESULT res;

  //todo: mount filesystem elsewhere
  res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
  printf("mount result: %d\n", res);

  osMutexAcquire(fileMutexHandle, osWaitForever);
  res = f_open(&SDFile, "temp_audio_file.wav", FA_CREATE_ALWAYS | FA_WRITE);
  printf("audio file open result: %d\n", res);

  WavHeader_Create(&header, RECORDING_LENGTH_MS);
  res = f_write(&SDFile, (const void*)&header, sizeof(WavHeader), (void *)&byteswritten);
  printf("audio file header write result: %d\n", res);
  file_write_index += byteswritten;

  f_close(&SDFile);
  osMutexRelease(fileMutexHandle);

  //let audio buf task start
  osSemaphoreRelease(audio_file_readyHandle);

  //wait until audio buf task data collection is finished
  osSemaphoreAcquire(audio_buf_finishedHandle, osWaitForever);

  char file_name[25];
  TimeStamp_GetTimeStampString(file_name);
  strcat(file_name, ".wav");

  printf("copying file...\n");
  osMutexAcquire(fileMutexHandle, osWaitForever);
  int copy_result = CopyFile("temp_audio_file.wav", file_name, (BYTE*)raw_rec_buf, sizeof(raw_rec_buf));
  osMutexRelease(fileMutexHandle);

  printf("copy_result: %d\n", copy_result);

  //todo: unmount filesystem elsewhere
  f_mount(&SDFatFS, (TCHAR const*)NULL, 0);

  osThreadExit();
}

void StartAudioBufTask(void *argument) {
  volatile int32_t * raw_rec_buf_part;
  uint32_t byteswritten;
  bool finished = false;

  osSemaphoreAcquire(audio_file_readyHandle, osWaitForever);

  while (1) {
    osMessageQueueGet(audioBufferReadyQueueHandle, (void*)&raw_rec_buf_part, NULL, osWaitForever);

    if (buf_full_count >= RECORDING_LENGTH_MS * AUDIO_SAMPLE_RATE / AUDIO_BUF_LEN / 1000) {
      HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0);
      printf("finished recording audio\n");
      finished = true;
    }

    //mutex protect processed_rec_buf and file writing
    osMutexAcquire(fileMutexHandle, osWaitForever);
    for (int i = 0; i<AUDIO_BUF_LEN/2; i++) {
      // full scale range with Sinc3 and FOSR set to 68 is +/- 68^3, approximately 2^18.26
      processed_rec_buf[i] = ((float)(raw_rec_buf_part[i]>>8)) / (2<<18); //only upper 24 bits are data
    }
    f_open(&SDFile, "temp_audio_file.wav", FA_WRITE);
    f_lseek(&SDFile, file_write_index);
    f_write(&SDFile, (const void*)processed_rec_buf, sizeof(processed_rec_buf), (void *)&byteswritten);
    f_close(&SDFile);
    file_write_index += byteswritten;
    osMutexRelease(fileMutexHandle);

    if (finished) {
      osSemaphoreRelease(audio_buf_finishedHandle);
      osThreadExit();
    }
  }
}


void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter){
  volatile int32_t * const raw_rec_buf_part = &raw_rec_buf[0][0];
  osMessageQueuePut(audioBufferReadyQueueHandle, (void*)&raw_rec_buf_part, osPriorityNormal, 0);
  //add warning of missed data if queuePut finds full queue?
}

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  buf_full_count++;
  volatile int32_t * const raw_rec_buf_part = &raw_rec_buf[1][0];
  osMessageQueuePut(audioBufferReadyQueueHandle, (void*)&raw_rec_buf_part, osPriorityNormal, 0);
  //add warning of missed data if queuePut finds full queue?
}
