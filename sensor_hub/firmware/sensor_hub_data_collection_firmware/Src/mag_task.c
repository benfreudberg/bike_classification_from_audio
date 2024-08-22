/*
 * mag_task.c
 *
 *  Created on: Aug 16, 2024
 *      Author: Fongberg
 */

#include "freertos_vars.h"
#include "spi.h"
#include "gpio.h"
#include "mmc5983ma_dev.h"
#include "tim.h"
#include "fatfs.h"
#include "timestamp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define NUM_MAGS        (2)
#define NUM_DATA_POINTS (2000) //2 seconds of data at 1kHz

static int collected_data[NUM_DATA_POINTS][NUM_MAGS][3];

typedef struct {
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    SPI_HandleTypeDef *hspi;
    osSemaphoreId_t *spi_semHandle;
} Mag;

static const Mag mag1 = {
    .cs_port = SPI1_CS1_GPIO_Port,
    .cs_pin = SPI1_CS1_Pin,
    .hspi = &hspi1,
    .spi_semHandle = &spi1_semHandle,
};

static const Mag mag2 = {
    .cs_port = SPI1_CS2_GPIO_Port,
    .cs_pin = SPI1_CS2_Pin,
    .hspi = &hspi1,
    .spi_semHandle = &spi1_semHandle,
};


static bool MagExists(const Mag *mag) {
  const uint8_t tx_data[] = {MAG_READ_REG(PROD_ID_REG), 0};
  uint8_t rx_data[2];

  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(mag->hspi, tx_data, rx_data, sizeof(tx_data));
  osSemaphoreAcquire(*mag->spi_semHandle, osWaitForever);
  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_SET);

  return rx_data[1] == PROD_ID;
}

static void SetMag(const Mag *mag) {
  const uint8_t tx_data[] = {MAG_WRITE_REG(INT_CTRL_0_REG), SET_OPERATION};
  uint8_t rx_data[2];

  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(mag->hspi, tx_data, rx_data, sizeof(tx_data));
  osSemaphoreAcquire(*mag->spi_semHandle, osWaitForever);
  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_SET);
}

static void ResetMag(const Mag *mag) {
  const uint8_t tx_data[] = {MAG_WRITE_REG(INT_CTRL_0_REG), RESET_OPERATION};
  uint8_t rx_data[2];

  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(mag->hspi, tx_data, rx_data, sizeof(tx_data));
  osSemaphoreAcquire(*mag->spi_semHandle, osWaitForever);
  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_SET);
}

static void SetMagSpeed(const Mag *mag, MagReadSpeed speed) {
  const uint8_t tx_data[] = {MAG_WRITE_REG(INT_CTRL_1_REG), speed};
  uint8_t rx_data[2];

  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(mag->hspi, tx_data, rx_data, sizeof(tx_data));
  osSemaphoreAcquire(*mag->spi_semHandle, osWaitForever);
  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_SET);
}

static void StartMagMeasurement(const Mag *mag) {
  const uint8_t tx_data[] = {MAG_WRITE_REG(INT_CTRL_0_REG), MEAS_M};
  uint8_t rx_data[2];

  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(mag->hspi, tx_data, rx_data, sizeof(tx_data));
  osSemaphoreAcquire(*mag->spi_semHandle, osWaitForever);
  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_SET);
}

static void ReadMagMeasurement(const Mag *mag, int readings[3]) {
  const uint8_t tx_data[8] = {MAG_READ_REG(X_OUT_0_REG), 0, 0, 0, 0, 0, 0, 0};
  uint8_t rx_data[8];
  uint8_t * const reading = &rx_data[1];

  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(mag->hspi, tx_data, rx_data, sizeof(tx_data));
  osSemaphoreAcquire(*mag->spi_semHandle, osWaitForever);
  HAL_GPIO_WritePin(mag->cs_port, mag->cs_pin, GPIO_PIN_SET);

  for (int j = 0; j < 3; j++) {
    uint32_t temporary_variable = (uint32_t) reading[2*j] << 10 |
                                  (uint32_t) reading[2*j + 1] << 2 |
                                             ((reading[6] >> (6 - 2*j)) & 0x3);
    readings[j] = (int) (temporary_variable - (1<<17));
  }
}

static void SaveMagDataToSdCard(void) {
  FRESULT res;
  char file_name[50]; //need 25 bytes
  TimeStamp_GetTimeStampString(file_name);
  strcat(file_name, ".csv");

  osMutexAcquire(fileMutexHandle, osWaitForever);
  res = f_open(&SDFile, file_name, FA_CREATE_ALWAYS | FA_WRITE);
  if (res) {
    printf("csv file open failed with result: %d\n", res);
    osMutexRelease(fileMutexHandle);
    osThreadExit();
  }

  f_printf(&SDFile, "mag1_x, mag1_y, mag1_z, mag2_x, mag2_y, mag2_z\n");
  for (int n = 0; n < NUM_DATA_POINTS; n++) {
    f_printf(&SDFile, "%d, %d, %d, %d, %d, %d\n", collected_data[n][0][0],
                                                  collected_data[n][0][1],
                                                  collected_data[n][0][2],
                                                  collected_data[n][1][0],
                                                  collected_data[n][1][1],
                                                  collected_data[n][1][2]);
  }

  f_close(&SDFile);
  osMutexRelease(fileMutexHandle);
}


void StartMagTask(void *argument) {
  bool mag_exists[NUM_MAGS];
  const Mag * const mags[] = {&mag1, &mag2};
  int readings[3];

  for (int i = 0; i < NUM_MAGS; i++) {
    mag_exists[i] = MagExists(mags[i]);
    printf("mag%d exists? %d\n", i+1, mag_exists[i]);
  }

#if 0
  for (int i = 0; i < NUM_MAGS; i++) {
    if (mag_exists[i]) {
      SetMag(mags[i]);
      ResetMag(mags[i]);
    }
  }
#endif

  for (int i = 0; i < NUM_MAGS; i++) {
    if (mag_exists[i]) {
      SetMagSpeed(mags[i], ODR800);
    }
  }

  for (int i = 0; i < NUM_MAGS; i++) {
    if (mag_exists[i]) {
      StartMagMeasurement(mags[i]);
    }
  }

  HAL_TIM_Base_Start_IT(&htim2);

  for (int n = 0; n < NUM_DATA_POINTS + 1; n++) {
    osSemaphoreAcquire(tim2_semHandle, osWaitForever);

    for (int i = 0; i < NUM_MAGS; i++) {
      if (mag_exists[i]) {
        ReadMagMeasurement(mags[i], readings);
        if (n) { //skip the first reading
          for (int j = 0; j < 3; j++) {
            collected_data[n-1][i][j] = readings[j];
          }
        }
      }
    }
    for (int i = 0; i < NUM_MAGS; i++) {
      if (mag_exists[i]) {
        StartMagMeasurement(mags[i]);
      }
    }

  }
  HAL_TIM_Base_Stop_IT(&htim2);
  printf("finished recording mag data\n");
  osSemaphoreRelease(i2c2_semHandle);

  /* data collection is done, so we can go low priority and wait for audio streaming
   * to finish before saving data to sd card
   */
  osThreadSetPriority(osThreadGetId(), osPriorityBelowNormal);
  while(osThreadGetState(audio_file_taskHandle) != osThreadTerminated) {
    osDelay(100);
  }
  osSemaphoreAcquire(file_system_readyHandle, osWaitForever);
  SaveMagDataToSdCard();

  printf("finished saving mag data\n");
  osSemaphoreRelease(task_finishedHandle);
  osThreadExit();
}

