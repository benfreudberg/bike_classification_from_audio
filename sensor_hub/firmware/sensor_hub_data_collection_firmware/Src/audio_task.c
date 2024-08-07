#include "main.h"
#include "dfsdm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "audio_task.h"
#include "freertos_vars.h"
#include "fatfs.h"
#include "timestamp.h"
#include <stdio.h>
#include <string.h>

#define RECORDING_LENGTH_MS (10000)

volatile int32_t raw_rec_buf[2][AUDIO_BUF_LEN/2];
static float processed_rec_buf[AUDIO_BUF_LEN/2];
static int buf_full_count = 0;
static FSIZE_t file_write_index = 0;

AudioBufPart audio_buf_1st_half = {
    .binary_sem_audio_buf_part_handle = &audio_buf_1st_data_readyHandle,
    .raw_rec_buf = &raw_rec_buf[0][0],
};

AudioBufPart audio_buf_2nd_half = {
    .binary_sem_audio_buf_part_handle = &audio_buf_2nd_data_readyHandle,
    .raw_rec_buf = &raw_rec_buf[1][0],
};

void StartAudioFileTask(void *argument) {
  WavHeader header;
  uint32_t byteswritten;
  FRESULT res;

  //todo: mount filesystem elsewhere
  res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
  printf("mount result: %d\n", res);

  osMutexAcquire(fileMutexHandle, portMAX_DELAY);
  res = f_open(&SDFile, "temp_audio_file.wav", FA_CREATE_ALWAYS | FA_WRITE);
  printf("audio file open result: %d\n", res);

  WavHeader_Create(&header, RECORDING_LENGTH_MS);
  res = f_write(&SDFile, (const void*)&header, sizeof(WavHeader), (void *)&byteswritten);
  printf("audio file header write result: %d\n", res);
  file_write_index += byteswritten;

  f_close(&SDFile);
  osMutexRelease(fileMutexHandle);

  //wait until data collection is finished
  osThreadSuspend(osThreadGetId());

  char file_name[25];
  TimeStamp_GetTimeStampString(file_name);
  strcat(file_name, ".wav");

  osMutexAcquire(fileMutexHandle, portMAX_DELAY);
  osThreadTerminate(audio_buf_taskHandle);
  osThreadTerminate(audio_buf_task2Handle);
  int copy_result = CopyFile("temp_audio_file.wav", file_name, (BYTE*)raw_rec_buf, sizeof(raw_rec_buf));
  osMutexRelease(fileMutexHandle);

  printf("copy_result: %d\n", copy_result);

  f_mount(&SDFatFS, (TCHAR const*)NULL, 0);

  osThreadExit();
}

void StartAudioBufTask(void *argument) {
  AudioBufPart * audio_buf = (AudioBufPart*) argument;
  osSemaphoreId_t * raw_rec_buf_data_ready_sem = audio_buf->binary_sem_audio_buf_part_handle;
  volatile int32_t * raw_rec_buf_part = audio_buf->raw_rec_buf;
  uint32_t byteswritten;
  FRESULT res;

  while (1) {
    osSemaphoreAcquire(*raw_rec_buf_data_ready_sem, portMAX_DELAY);

    if (buf_full_count >= RECORDING_LENGTH_MS * AUDIO_SAMPLE_RATE / AUDIO_BUF_LEN / 1000) {
      HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0);
      printf("finished recording audio\n");
      osThreadResume(audioFileTaskHandle);
    }

    //mutex protect processed_rec_buf and file writing
    osMutexAcquire(fileMutexHandle, portMAX_DELAY);
    for (int i = 0; i<AUDIO_BUF_LEN/2; i++) {
      // full scale range with Sinc3 and FOSR set to 68 is +/- 68^3, approximately 2^18.26
      processed_rec_buf[i] = ((float)(raw_rec_buf_part[i]>>8)) / (2<<18); //only upper 24 bits are data
    }
    f_open(&SDFile, "temp_audio_file.wav", FA_WRITE);
    f_lseek(&SDFile, file_write_index);
    res = f_write(&SDFile, (const void*)processed_rec_buf, sizeof(processed_rec_buf), (void *)&byteswritten);
    f_close(&SDFile);
    file_write_index += byteswritten;
    osMutexRelease(fileMutexHandle);
//    printf("writing data result %d, buffer filled %d times\n", res, buf_full_count);
  }
}


void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter){
  osSemaphoreRelease(audio_buf_1st_data_readyHandle);
}

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  buf_full_count++;
  osSemaphoreRelease(audio_buf_2nd_data_readyHandle);
}
