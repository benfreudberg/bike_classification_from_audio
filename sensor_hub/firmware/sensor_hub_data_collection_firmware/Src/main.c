/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dfsdm.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "sdmmc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AUDIO_BUF_LEN   10240
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int32_t rec_buf[AUDIO_BUF_LEN];
float save_buf[AUDIO_BUF_LEN/2];
bool dma_rec_half_buf_cplt = false;
bool dma_rec_buf_cplt = false;
int dma_rec_buf_cplt_count = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DFSDM1_Init();
  MX_I2C2_Init();
  MX_SDMMC1_SD_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_FATFS_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  //microphone
  HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1_filter0, rec_buf, AUDIO_BUF_LEN);

  //enable backup battery charging
  HAL_PWREx_EnableBatteryCharging(PWR_BATTERY_CHARGING_RESISTOR_5);

  //mags
  const uint8_t tx_data[] = {0x2f|0x80,0};
  uint8_t rx_data1[] = {0,0};
  uint8_t rx_data2[] = {0,0};

  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data1, sizeof(tx_data), 1); //prime spi port

  HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data1, sizeof(tx_data), 1);
  HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(GPIOC, SPI1_CS2_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data2, sizeof(tx_data), 1);
  HAL_GPIO_WritePin(GPIOC, SPI1_CS2_Pin, GPIO_PIN_SET);

  printf("spi1_cs1 read from 0x2f: 0x%02x\n", rx_data1[1]);
  printf("spi1_cs2 read from 0x2f: 0x%02x\n", rx_data2[1]);

  //cam spi
  const uint8_t tx_data_cam_write[] = {0x0|0x80,0x4b};
  const uint8_t tx_data_cam_read[] = {0x0|0x00,0x00};
  uint8_t rx_data_cam[] = {0,0};

  HAL_SPI_TransmitReceive(&hspi2, tx_data_cam_write, rx_data_cam, sizeof(tx_data), 1); //prime spi port

  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi2, tx_data_cam_write, rx_data_cam, sizeof(tx_data), 1);
  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi2, tx_data_cam_read, rx_data_cam, sizeof(tx_data), 1);
  HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
  printf("cam read from 0x00: 0x%02x\n", rx_data_cam[1]);

  //cam i2c (sequential read not supported)
  uint8_t tx_data_cam_i2c_set_to_reg_bank_2[] = {0xff, 0x01};
  uint8_t tx_data_cam_i2c_set_reg_to_read_to_idA = 0x1c;
  uint8_t tx_data_cam_i2c_set_reg_to_read_to_idB = 0x1d;
  uint8_t rx_data_cam_i2c_idA, rx_data_cam_i2c_idB;
  HAL_I2C_Master_Transmit(
      &hi2c2,
      0x30<<1,
      tx_data_cam_i2c_set_to_reg_bank_2,
      sizeof(tx_data_cam_i2c_set_to_reg_bank_2),
      1);
  HAL_I2C_Master_Transmit(
      &hi2c2,
      0x30<<1,
      &tx_data_cam_i2c_set_reg_to_read_to_idA,
      sizeof(tx_data_cam_i2c_set_reg_to_read_to_idA),
      1);
  HAL_I2C_Master_Receive(
      &hi2c2,
      0x30<<1,
      &rx_data_cam_i2c_idA,
      sizeof(rx_data_cam_i2c_idA),
      1);
  HAL_I2C_Master_Transmit(
      &hi2c2,
      0x30<<1,
      &tx_data_cam_i2c_set_reg_to_read_to_idB,
      sizeof(tx_data_cam_i2c_set_reg_to_read_to_idB),
      1);
  HAL_I2C_Master_Receive(
      &hi2c2,
      0x30<<1,
      &rx_data_cam_i2c_idB,
      sizeof(rx_data_cam_i2c_idB),
      1);
  uint16_t cam_i2c_id = (uint16_t)rx_data_cam_i2c_idA<<8 | rx_data_cam_i2c_idB;
  printf("cam i2c ID: 0x%04x\n", cam_i2c_id);

  //sd card file system
//  FRESULT res; /* FatFs function common result code */
//  uint32_t byteswritten, bytesread; /* File write/read counts */
//  uint8_t wtext[] = "STM32 FATFS works great!"; /* File write buffer */
//  uint8_t rtext[_MAX_SS];/* File read buffer */
//
//  res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
//  printf("res: %d\n", res);
//  if(res != FR_OK)
//   {
//       Error_Handler();
//   }
//   else
//   {
//     res = f_mkfs((TCHAR const*)SDPath, FM_FAT32, 0, rtext, sizeof(rtext));
//     printf("res: %d\n", res);
//     if(res != FR_OK)
//     {
//         Error_Handler();
//     }
//     else
//     {
//       //Open file for writing (Create)
//       res = f_open(&SDFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE);
//       printf("res: %d\n", res);
//       if(res != FR_OK)
//       {
//           Error_Handler();
//       }
//       else
//       {
//         //Write to the text file
//         res = f_write(&SDFile, wtext, strlen((char *)wtext), (void *)&byteswritten);
//         printf("res: %d\n", res);
//         if((byteswritten == 0) || (res != FR_OK))
//         {
//           Error_Handler();
//         }
//         else
//         {
//           f_close(&SDFile);
//         }
//       }
//     }
//   }
//  f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
//  res = f_open(&SDFile, "audio_sample_long_file_name.wav", FA_CREATE_ALWAYS | FA_WRITE); //max 8 chars before '.'?
//  printf("res: %d\n", res);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t tick;
  int light_state = 0;
  int last_light_state = -1;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    tick = HAL_GetTick();
//    if (dma_rec_buf_cplt_count > 1) { //skip the first 2 buffer-fulls of data (232 ms each at 10240 size)
//      if (dma_rec_half_buf_cplt) {
//        //it takes about 15 ms to do this conversion and saving (mostly saving) for 116 ms of audio data
//        for (int i = 0; i<AUDIO_BUF_LEN/2; i++) {
//          // full scale range with Sinc3 and FOSR set to 68 is +/- 68^3, approximately 2^18.26
//          save_buf[i] = ((float)(rec_buf[i]>>8)) / (2<<18);
//        }
//        dma_rec_half_buf_cplt = false;
//        f_write(&SDFile, (const void*)save_buf, sizeof(save_buf), (void *)&byteswritten);
//      }
//      if (dma_rec_buf_cplt) {
//        for (int i = 0; i<AUDIO_BUF_LEN/2; i++) {
//          save_buf[i] = ((float)(rec_buf[i + AUDIO_BUF_LEN/2]>>8)) / (2<<18);
//        }
//        dma_rec_buf_cplt = false;
//        f_write(&SDFile, (const void*)save_buf, sizeof(save_buf), (void *)&byteswritten);
//      }
//    }

    light_state = (tick%3000) / 1000;
    if (light_state != last_light_state) {
      last_light_state = light_state;
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
    }
    if (tick%4111 == 0) {
      RTC_TimeTypeDef time;
      RTC_DateTypeDef date;
      HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
      HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
      printf("%02d/%02d/%04d %02d:%02d:%02d\n", date.Month, date.Date, 1980 + date.Year, time.Hours, time.Minutes, time.Seconds);
    }
    if (dma_rec_buf_cplt_count > 65) { //end after 66-2 total buffers (14.85 seconds)
      HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0);
      f_close(&SDFile);
      f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
      return 0;
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

int _write(int file, char *ptr, int len)
{
  (void)file;

  HAL_UART_Transmit(&huart2, (const uint8_t *)ptr, len, 10);
  return len;
}

void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter){
  dma_rec_half_buf_cplt = true;
}

void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
  dma_rec_buf_cplt = true;
  dma_rec_buf_cplt_count++;
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
