#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

#include "cmsis_os.h"
#include "wav_header.h"

#define AUDIO_BUF_LEN   (AUDIO_SAMPLE_RATE/5*2) // 200ms per half

typedef struct _AudioBufPart {
    osSemaphoreId_t * binary_sem_audio_buf_part_handle;
    volatile int32_t * raw_rec_buf;
} AudioBufPart;

extern volatile int32_t raw_rec_buf[2][AUDIO_BUF_LEN/2];
extern AudioBufPart audio_buf_1st_half;
extern AudioBufPart audio_buf_2nd_half;

#endif /* AUDIO_TASK_H_ */
