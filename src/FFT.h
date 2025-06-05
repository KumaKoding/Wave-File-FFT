#ifndef __FFT_H__
#define __FFT_H__

#include <stdio.h>
#include <stdlib.h>
#include "decode_wav.h"

typedef struct complexNumber
{
    double real;
    double imag;
} complexNumber;

struct fourierSeries
{
    size_t len;
    double binWidth;
    complexNumber *series;
};

struct fourierSeries *fastFourierTransform(waveFile *signal);

#endif