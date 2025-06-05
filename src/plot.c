#include "plot.h"

int sign(int n)
{
    if(n < 0)
    {
        return -1;
    }
    else if(n > 0)
    {
        return 1;
    }

    return 0;
}

uint8_t *plot(waveFile *data, int width, int height)
{
    uint8_t *pixels = malloc(width * height * 3);

    uint32_t trueWidth = data->dataSize / data->sampleSize;

    int deltaX = (int)ceil(trueWidth / width);
    int deltaY = (int)ceil((double)0xffff / (double)height);
    
    for(int i = 0; i < (width * height * 3); i++)
    {
        pixels[i] = 255;
    }

    for(int x = 0; x < width; x++)
    {
        int acc = 0;

        for(int i = 0; i < deltaX; i++)
        {   
            acc += data->data[(x * deltaX) + i];
        }

        acc = acc / deltaX;

        for(int y = 0; y < abs(acc) / deltaY; y++)
        {
            pixels[(x * 3) + (((height / 2) + (acc / deltaY) - (y * sign(acc))) * width * 3)] = 0;
            pixels[(x * 3) + (((height / 2) + (acc / deltaY) - (y * sign(acc))) * width * 3) + 1] = 255;
            pixels[(x * 3) + (((height / 2) + (acc / deltaY) - (y * sign(acc))) * width * 3) + 2] = 0;
        }
    }

    return pixels;
}