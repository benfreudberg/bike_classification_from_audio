#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

#include "cmsis_os.h"

//defined by system clock speed (120MHz), dfsdm clock divider (40), and dfsdm filter oversampling (68)
#define AUDIO_SAMPLE_RATE (44100) //really 44118
#define NUM_CHANNELS      (1)
#define AUDIO_BUF_LEN     (AUDIO_SAMPLE_RATE/5*2) // 200ms per half

extern volatile int32_t raw_rec_buf[2][AUDIO_BUF_LEN/2];

#endif /* AUDIO_TASK_H_ */
