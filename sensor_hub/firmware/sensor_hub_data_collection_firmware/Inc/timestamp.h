#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include "integer.h"

void TimeStamp_AquireTime();
void TimeStamp_GetTimeStampString(char str[]);
DWORD TimeStamp_GetFatTime();

#endif /* TIMESTAMP_H_ */
