#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include <stdio.h>

void StartLedsTask(void *argument)
{
  int light_state = 0;
  uint32_t scheduling_kernel_tick_count = 0;

  while(1) {
    switch (light_state) {
      case 0:
        HAL_GPIO_WritePin(GPIOE, LED3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOE, LED1_Pin, GPIO_PIN_SET);
        printf("green\n");
        break;
      case 1:
        HAL_GPIO_WritePin(GPIOE, LED1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOE, LED2_Pin, GPIO_PIN_SET);
        printf("yellow\n");
        break;
      case 2:
        HAL_GPIO_WritePin(GPIOE, LED2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOE, LED3_Pin, GPIO_PIN_SET);
        printf("red\n");
    }
    light_state++;
    light_state%=3;

    scheduling_kernel_tick_count += 1000;
    osDelayUntil(scheduling_kernel_tick_count);
  }
}
