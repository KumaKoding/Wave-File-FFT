#ifndef __DECODE_WAV_H__
#define __DECODE_WAV_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

typedef struct waveFile {
    uint32_t sampleRate;
    uint16_t sampleSize;
    uint32_t dataSize;

    int16_t *data;
} waveFile;

waveFile *decode(uint8_t *buf);

#endif