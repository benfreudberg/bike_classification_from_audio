/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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
#include "timestamp.h"
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return TimeStamp_GetFatTime();
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
int CopyFile(char *srcFile, char *destFile, BYTE buffer[], UINT buffer_length)
{
  FIL file;
  FRESULT res;
  UINT br = 0, bw = 0;
  FSIZE_t bytes_copied = 0;

  while(1) {
    res = f_open(&file, (const TCHAR*)srcFile, FA_READ | FA_OPEN_EXISTING);
    if(res) {
      return -1;
    }
    res = f_lseek(&file, bytes_copied);
    if(res) {
      return -2;
    }

    f_read(&file, buffer, buffer_length, &br);  /* Read a chunk of source file */
    f_close(&file);

    if(!br) {
      return 0;
    }

    if(br) {
      res = f_open(&file, (const TCHAR*)destFile, FA_CREATE_ALWAYS | FA_WRITE);
      if(res) {
        return -3;
      }
      res = f_lseek(&file, bytes_copied);
      if(res) {
        return -4;
      }

      f_write(&file, buffer, br, &bw); /* Write it to the destination file */
      f_close(&file);
    }
    bytes_copied += bw;
  }
}
/* USER CODE END Application */
