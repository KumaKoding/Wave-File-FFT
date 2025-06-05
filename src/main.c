#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "decode_wav.h"
#include "FFT.h"
#include "plot.h"
#include "encode_bmp.h"

#define WIDTH 5000
#define HEIGHT 3000
#define MIDDLE (HEIGHT / 2)

size_t fileSize(FILE *f)
{
    fseek(f, 0L, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0L, SEEK_SET);

    return size;
}

int main(int argc, char *argv[])
{
    FILE *f;
    int size;

    if(argc < 2)
    {
        return -1;
    }

    f = fopen(argv[1], "r");
    size = fileSize(f);

    uint8_t *buf = malloc(size);
    fread(buf, sizeof(uint8_t), size, f);

    waveFile *data = decode(buf);

    struct fourierSeries *fourierSeries = fastFourierTransform(data);
    waveFile *fourierSeriesWave = malloc(sizeof(waveFile));

    fourierSeriesWave->data = malloc(sizeof(int16_t) * (fourierSeries->len / 2));
    fourierSeriesWave->dataSize = sizeof(int16_t) * (fourierSeries->len / 2);
    fourierSeriesWave->sampleSize = sizeof(int16_t);
    fourierSeriesWave->sampleRate = 0;

    double maxAmplitude = 0.0f;
    for(int i = 0; i < fourierSeries->len / 2; i++)
    {
        double amplitude = sqrt((fourierSeries->series[i].real * fourierSeries->series[i].real) + (fourierSeries->series[i].imag * fourierSeries->series[i].imag));

        if(amplitude > maxAmplitude)
        {
            maxAmplitude = amplitude;
        }
    }

    double xm = 0.0f;
    double m = 0.0f;

    double I = 0.0f;
    double I_I = 0.0f;
    for(int i = 0; i < fourierSeries->len / 2; i++)
    {
        double amplitude = sqrt((fourierSeries->series[i].real * fourierSeries->series[i].real) + (fourierSeries->series[i].imag * fourierSeries->series[i].imag));
        I += amplitude * fourierSeries->binWidth;
        fourierSeriesWave->data[i] = (int16_t)(((amplitude) / 2000000.0f) * INT16_MAX);

        m += amplitude;
        xm += amplitude * i;

        I_I += I;
    }
    // 2539152.60515
    // 253792549.64
    // 991276344.055084

    printf("Domain = %zu\n", fourierSeries->len / 2);
    printf("Area Under Curve = %f\n", I);
    printf("Integral of the Area = %f\n", I_I);
    printf("Maximum Amplitude = %f\n", maxAmplitude);
    printf("COA = %f\n", xm / m);

    encode(plot(fourierSeriesWave, WIDTH, HEIGHT), WIDTH, HEIGHT, argv[2]);
    encode(plot(data, WIDTH, HEIGHT), WIDTH, HEIGHT, "sound.bmp");

    free(data->data);
    free(data);
    free(fourierSeries);
    free(fourierSeriesWave->data);
    free(fourierSeriesWave);
    fclose(f);

    return 0;
}