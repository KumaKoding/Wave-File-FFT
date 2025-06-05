#include "FFT.h"

#include <math.h>

struct division
{
    int index;
    int samples;
    int jump;
};

complexNumber complexMultiplication(complexNumber factor1, complexNumber factor2)
{
    // (a_0 + b_0i)(a_1 + b_1i) = (a_0)(a_1) + ((a_0)(b_1) + (a_1)(b_0))i - (b_0)(b_1)

    complexNumber z;

    z.real = (factor1.real * factor2.real) - (factor1.imag * factor2.imag);
    z.imag = (factor1.real * factor2.imag) + (factor2.real * factor1.imag);

    return z;
}

complexNumber *decompose(int16_t *points, size_t len)
{

    // // 
    // // Decompose array into bit-reversed order
    // // x_0, x_1, x_2, x_3, x_4, x_5, x_6, x_7 --> x_0, x_4, x_2, x_6, x_1, x_5, x_3, x_7
    // // 

    struct division divisions[4096] = {0};
    
    divisions[0] = (struct division){.index = 0, .samples = len, .jump = 1};
    int remainingDivisions = 1;

    complexNumber *decompositions = malloc(sizeof(complexNumber) * len);

    int i = 0;
    
    while(remainingDivisions > 0)
    {

        struct division currentDivision = divisions[remainingDivisions - 1];
        remainingDivisions--;

        if (currentDivision.samples == 1)
        {
            // base case
            decompositions[i].real = (double)points[currentDivision.index];
            decompositions[i].imag = 0;

            i++;
        }
        else
        {
            // even must be evaluated first, so it goes after the odd one
            divisions[remainingDivisions] = (struct division){
                .index = currentDivision.index + currentDivision.jump, 
                .jump = currentDivision.jump * 2, 
                .samples = currentDivision.samples / 2
            };
            
            remainingDivisions++;

            divisions[remainingDivisions] = (struct division){
                .index = currentDivision.index, 
                .jump = currentDivision.jump * 2, 
                .samples = currentDivision.samples / 2
            };

            remainingDivisions++;
        }
    }

    return decompositions;
}

void printComplex(complexNumber z)
{
    printf("%f+%fi", z.real, z.imag);
}

struct fourierSeries *fastFourierTransform(waveFile *signal)
{
    struct fourierSeries *F = malloc(sizeof(struct fourierSeries));
    uint32_t nearestPowerOf2 = 1;
    
    while(nearestPowerOf2 < signal->dataSize / signal->sampleSize)
    {
        nearestPowerOf2 = nearestPowerOf2 << 1;
    }

    int16_t *samples = malloc(sizeof(int16_t) * nearestPowerOf2);
    size_t nSamples = nearestPowerOf2;

    for(int i = 0; i < nearestPowerOf2; i++)
    {
        if(i < signal->dataSize / signal->sampleSize)
        {
            samples[i] = signal->data[i];
        }
        else
        {
            samples[i] = 0;
        }
    }

    complexNumber *fourierSeries = malloc(sizeof(complexNumber) * nSamples);
    complexNumber *decompositions = decompose(samples, nSamples);

    //
    // Radix-2 DIT FFT
    // Iterative, depth first
    //

    complexNumber *oldLayer = decompositions;
    complexNumber *newLayer = NULL;

    int N = 2; // the base case is already fulfilled as each sample is its own DFT

    while(N <= nSamples)
    {
        newLayer = malloc(sizeof(complexNumber) * nSamples);

        for(int n = 0; n < (nSamples / N); n++)
        {
            // Because we are creating a DFT of N=2, the periodicity exists for every other integer.
            // As the number of samples in each DFT increases (doubles), the k value will increment
            // to a value half that size.

            for(int i = 0; i < (N / 2); i++)
            {
                int k = i;
                int indexE = (n * N) + i;
                int indexO = (n * N) + (N / 2) + i;

                complexNumber w = (complexNumber){
                    .real = cos((-2 * M_PI * k) / N),
                    .imag = sin((-2 * M_PI * k) / N)
                };

                complexNumber oddComponent = complexMultiplication(oldLayer[indexO], w);

                
                newLayer[indexE].real = oldLayer[indexE].real + oddComponent.real;
                newLayer[indexE].imag = oldLayer[indexE].imag + oddComponent.imag;
                
                newLayer[indexO].real = oldLayer[indexE].real - oddComponent.real;
                newLayer[indexO].imag = oldLayer[indexE].imag - oddComponent.imag;
            }
        }
        
        free(oldLayer);
        oldLayer = newLayer;

        N = N << 1;
    }

    fourierSeries = newLayer;

    F->len = nSamples;
    F->binWidth = ((double)signal->dataSize / (double)signal->sampleSize) / (double)nSamples;
    F->series = fourierSeries;

    return F;
}
