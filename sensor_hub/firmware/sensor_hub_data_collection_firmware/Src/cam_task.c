/*
 * cam_task.c
 *
 *  Created on: Aug 21, 2024
 *      Author: Fongberg
 */


#include "arducam.h"
#include "arducam_dev_sensor.h"
#include "freertos_vars.h"
#include <stdio.h>

#define BUFFER_SIZE 10240
static uint8_t buffer[BUFFER_SIZE]; //10kB buffer

const Arducam cam = {
    .cs_port = SPI2_CS_GPIO_Port,
    .cs_pin = SPI2_CS_Pin,
    .hspi = &hspi2,
    .spi_semHandle = &spi2_semHandle,

    .hi2c = &hi2c2,
    .i2c_addr = 0x30,
    .i2c_semHandle = &i2c2_semHandle,
};

void StartCamTask(void *argument) {
  uint32_t image_size;
  uint8_t test_reg_val_write = 0x4b;
  uint16_t cam_sensor_pid = ArducamSensorGetPID(&cam);
  ArducamChipWriteTestReg(&cam, test_reg_val_write);
  uint8_t test_reg_val_read = ArducamChipReadTestReg(&cam);

  if (cam_sensor_pid != ((OV2640_SENSOR_ID_HIGH_VAL << 8) | OV2640_SENSOR_ID_LOW_VAL)) {
    printf("ERROR: cam sensor not found\n");
    osSemaphoreRelease(task_finishedHandle);
    osThreadExit();
  }

  if (test_reg_val_read != test_reg_val_write) {
    printf("ERROR: cam chip not found\n");
    osSemaphoreRelease(task_finishedHandle);
    osThreadExit();
  }

  ArducamInit(&cam);
  ArducamCapture(&cam);

  osThreadSetPriority(osThreadGetId(), osPriorityBelowNormal1);
  image_size = ArducamChipGetFifoSize(&cam);
  osSemaphoreAcquire(file_system_readyHandle, osWaitForever);
  ArducamReadAndSaveImage(&cam, buffer, BUFFER_SIZE, image_size);

  //todo: put cam into low power state
  printf("cam task finished\n");
  osSemaphoreRelease(task_finishedHandle);
  osThreadExit();
}
