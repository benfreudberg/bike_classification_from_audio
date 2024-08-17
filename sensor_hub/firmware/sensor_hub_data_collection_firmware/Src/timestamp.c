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
  sprintf(str, "%04d-%02d-%02d--%02d-%02d-%02d", 1980 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
}

DWORD TimeStamp_GetFatTime() {
  union {
    struct {
        DWORD second : 5;
        DWORD minute : 6;
        DWORD hour   : 5;
        DWORD day    : 5;
        DWORD month  : 4;
        DWORD year   : 7;
    } date_time_struct;
    DWORD date_time_dword;
  } date_time_union;

  if(!time_aquired) {
    TimeStamp_AquireTime();
  }
  date_time_union.date_time_struct.second = time.Seconds / 2;
  date_time_union.date_time_struct.minute = time.Minutes;
  date_time_union.date_time_struct.hour   = time.Hours;
  date_time_union.date_time_struct.day    = date.Date;
  date_time_union.date_time_struct.month  = date.Month;
  date_time_union.date_time_struct.year   = date.Year;

  return date_time_union.date_time_dword;
}
