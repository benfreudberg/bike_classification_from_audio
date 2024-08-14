#include "wav_header.h"
#include "audio_task.h"
#include <string.h>

void WavHeader_Create(WavHeader * header, uint32_t length_ms) {
  uint32_t data_bytes = NUM_CHANNELS *
      AUDIO_SAMPLE_RATE * sizeof(float) * length_ms / 1000;

  memcpy(header->riff_header, "RIFF", 4);
  header->file_size = data_bytes + sizeof(WavHeader) - 8;
  memcpy(header->wave_header, "WAVE", 4);

  memcpy(header->fmt_header, "fmt ", 4);
  header->fmt_chunk_size = 16;
  header->audio_format = 3;
  header->num_channels = NUM_CHANNELS;
  header->sample_rate = AUDIO_SAMPLE_RATE;
  header->byte_rate = NUM_CHANNELS * AUDIO_SAMPLE_RATE * sizeof(float);
  header->sample_alignment = NUM_CHANNELS * sizeof(float);
  header->bit_depth = sizeof(float) * 8;

  memcpy(header->data_header, "data", 4);
  header->data_bytes = data_bytes;
}
