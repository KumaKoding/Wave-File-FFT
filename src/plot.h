#ifndef __PLOT_H__
#define __PLOT_H__

#include <stdio.h>
#include <stdlib.h>

#include "decode_wav.h"

uint8_t *plot(waveFile *data, int width, int height);

#endif