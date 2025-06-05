#include "decode_wav.h"

#define SIZE_CHUNK_ID 4 // B
#define SIZE_CHUNK_SIZE 4 // L
#define SIZE_FORMAT 4 // B

#define SIZE_SUB_CHUNK_ID 4 // B
#define SIZE_SUB_CHUNK_SIZE 4 // B

#define SIZE_AUDIO_FORMAT 2 // L
#define SIZE_NUM_CHANNELS 2 // L
#define SIZE_SAMPLE_RATE 4 // L
#define SIZE_BYTE_RATE 4 // L
#define SIZE_BLOCK_ALIGN 2 // L
#define SIZE_BITS_PER_SAMPLE 2 // L

uint8_t *splice(int startIndex, int endIndex, uint8_t *buf)
{
    uint8_t *bytes = malloc(sizeof(uint8_t) * (endIndex - startIndex));

    for(int i = 0; i < endIndex - startIndex; i++)
    {
        bytes[i] = buf[startIndex + i];
    }

    return bytes;
}

uint8_t *scrape(int *ptr, size_t nBytes, uint8_t *buf)
{
    uint8_t *spl = splice(*ptr, *ptr + nBytes, buf);
    *ptr += nBytes;

    return spl;
}


uint32_t LEInt(uint8_t *bytes, size_t size)
{
    uint32_t integer = 0x00000000;

    for(int i = 0; i < size; i++)
    {
        integer = integer << 8;
        integer = integer | (bytes[size - i - 1]);
    }

    return integer;
}

int spliceComp(uint8_t *spl, char const *str, size_t len)
{
    for(int i = 0; i < len; i++)
    {
        if(spl[i] != str[i])
        {
            return 0;
        }
    }

    return 1;
}

void seekNewSubChunk(int *c, uint8_t *buf, char const *id)
{
    uint8_t *newChunk = scrape(c, SIZE_SUB_CHUNK_ID, buf);

    while(!spliceComp(newChunk, id, SIZE_SUB_CHUNK_ID))
    {
        uint8_t *newSize = scrape(c, SIZE_SUB_CHUNK_SIZE, buf);
        *c += LEInt(newSize, SIZE_SUB_CHUNK_SIZE);
        
        free(newChunk);
        free(newSize);

        newChunk = scrape(c, SIZE_SUB_CHUNK_ID, buf);
    }

    free(newChunk);
}

waveFile *decode(uint8_t *buf)
{
    int c = 0;

    uint8_t *chunkID = scrape(&c, SIZE_CHUNK_ID, buf);
    uint8_t *chunkSize = scrape(&c, SIZE_CHUNK_SIZE, buf);
    uint8_t *format = scrape(&c, SIZE_FORMAT, buf);

    seekNewSubChunk(&c, buf, "fmt ");

    uint8_t *subChunk1Size = scrape(&c, SIZE_SUB_CHUNK_SIZE, buf);
    uint8_t *audioFormat = scrape(&c, SIZE_AUDIO_FORMAT, buf);
    uint8_t *numChannels = scrape(&c, SIZE_NUM_CHANNELS, buf);
    uint8_t *sampleRate = scrape(&c, SIZE_SAMPLE_RATE, buf);
    uint8_t *byteRate = scrape(&c, SIZE_BYTE_RATE, buf);
    uint8_t *blockAlign = scrape(&c, SIZE_BLOCK_ALIGN, buf);
    uint8_t *bitsPerSample = scrape(&c, SIZE_BITS_PER_SAMPLE, buf);
    
    assert(spliceComp(chunkID, "RIFF", SIZE_CHUNK_ID));
    assert(spliceComp(format, "WAVE", SIZE_FORMAT));
    assert(LEInt(audioFormat, SIZE_AUDIO_FORMAT) == 1);
    assert(LEInt(numChannels, SIZE_NUM_CHANNELS) == 1);
    
    seekNewSubChunk(&c, buf, "data");
    
    uint8_t *subChunk2Size = scrape(&c, SIZE_SUB_CHUNK_SIZE, buf);

    uint32_t hertz = LEInt(sampleRate, SIZE_SAMPLE_RATE);
    uint16_t sampleSize = LEInt(bitsPerSample, SIZE_BITS_PER_SAMPLE) / 8;
    uint32_t dataSize = LEInt(subChunk2Size, SIZE_SUB_CHUNK_SIZE);
    uint32_t samples = dataSize / sampleSize;

    assert(sampleSize == 2);

    int16_t *data = malloc(dataSize * 2);

    for(int i = 0; i < samples; i++)
    {
        uint64_t currentIndex = c + (i * 2);
        data[i] = ((0x0000 | buf[currentIndex + 1]) << 8) | buf[currentIndex];
    }
    
    free(chunkID);
    free(chunkSize);
    free(format);
    free(subChunk1Size);
    free(audioFormat);
    free(numChannels);
    free(sampleRate);
    free(byteRate);
    free(blockAlign);
    free(bitsPerSample);
    free(subChunk2Size);

    waveFile *wave = malloc(sizeof(waveFile));

    wave->sampleRate = hertz;
    wave->sampleSize = sampleSize;
    wave->dataSize = dataSize;
    wave->data = data;

    return wave;
}