#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

#include "cmsis_os.h"

//defined by system clock speed (120MHz), dfsdm clock divider (40), and dfsdm filter oversampling (68)
#define AUDIO_SAMPLE_RATE  (44100) //really 44118
#define NUM_CHANNELS       (1)
#define AUDIO_BUF_LEN      (AUDIO_SAMPLE_RATE/20*2) //50ms per half
#define AUDIO_BUF_HALF_LEN (AUDIO_BUF_LEN/2)

extern int32_t raw_rec_buf[2][AUDIO_BUF_HALF_LEN];

#endif /* AUDIO_TASK_H_ */
