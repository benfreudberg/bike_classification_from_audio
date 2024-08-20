#include "rtc.h"
#include "timestamp.h"
#include "bit_manipulation.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static RTC_TimeTypeDef time;
static RTC_DateTypeDef date;
static bool time_aquired = false;

void TimeStamp_AquireTime() {
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  time_aquired = true;
}

void TimeStamp_GetTimeStampString(char str[]) {
  if(!time_aquired) {
    TimeStamp_AquireTime();
  }
  sprintf(str, "%04d-%02d-%02d--%02d-%02d-%02d", 1980 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
}

DWORD TimeStamp_GetFatTime() {
  if(!time_aquired) {
    TimeStamp_AquireTime();
  }

  DWORD date_time = 0;
  SET_FIELD(date_time, 5, 0, time.Seconds / 2);
  SET_FIELD(date_time, 6, 5, time.Minutes);
  SET_FIELD(date_time, 5, 11, time.Hours);
  SET_FIELD(date_time, 5, 16, date.Date);
  SET_FIELD(date_time, 4, 21, date.Month);
  SET_FIELD(date_time, 7, 25, date.Year);

  return date_time;
}
