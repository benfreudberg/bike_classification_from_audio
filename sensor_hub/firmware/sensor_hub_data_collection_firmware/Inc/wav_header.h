//adapted from https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f

#ifndef WAV_HEADER_H_
#define WAV_HEADER_H_

#include <assert.h>
#include <stdint.h>

typedef struct _WavHeader {
    // RIFF Header
    uint8_t  riff_header[4]; // Contains "RIFF"
    uint32_t file_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    uint8_t  wave_header[4]; // Contains "WAVE"

    // Format Header
    uint8_t  fmt_header[4]; // Contains "fmt " (includes trailing space)
    uint32_t fmt_chunk_size; // Should be 16 for PCM
    uint16_t audio_format; // Should be 1 for PCM. 3 for IEEE Float
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    uint16_t sample_alignment; // num_channels * Bytes Per Sample
    uint16_t bit_depth; // Number of bits per sample

    // Data
    uint8_t  data_header[4]; // Contains "data"
    uint32_t data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
} WavHeader;

static_assert(sizeof(WavHeader) == 44, "WavHeader defined incorrectly");

void WavHeader_Create(WavHeader * header, uint32_t length_ms);

#endif /* WAV_HEADER_H_ */
