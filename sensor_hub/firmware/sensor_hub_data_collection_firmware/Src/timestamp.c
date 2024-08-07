#include "rtc.h"
#include "timestamp.h"
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
  sprintf(str, "%04d-%02d-%02d_%02d-%02d-%02d", 1980 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
}

DWORD TimeStamp_GetFatTime() {
  struct {
      DWORD second : 5;
      DWORD minute : 6;
      DWORD hour   : 5;
      DWORD day    : 5;
      DWORD month  : 4;
      DWORD year   : 7;
  } date_time;

  if(!time_aquired) {
    TimeStamp_AquireTime();
  }
  date_time.second = time.Seconds / 2;
  date_time.minute = time.Minutes;
  date_time.hour   = time.Hours;
  date_time.day    = date.Date;
  date_time.month  = date.Month;
  date_time.year   = date.Year;

  DWORD ret;
  memcpy(&ret, &date_time, sizeof(DWORD));
  return ret;
}
