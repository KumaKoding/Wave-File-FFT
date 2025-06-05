#include "encode_bmp.h"

#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

#define SIGNATURE (uint8_t[]){0x42, 0x4D}
#define RESERVED (uint8_t[]){0x00, 0x00, 0x00, 0x00}
#define DATA_OFFSET (HEADER_SIZE + INFO_HEADER_SIZE)

#define PLANES (uint8_t[]){0x01, 0x00}
#define BITS_PER_PIXEL (uint8_t[]){0x18, 0x00}
#define COMPRESSION (uint8_t[]){0x00, 0x00, 0x00, 0x00}
#define COMPRESSED_SIZE (uint8_t[]){0x00, 0x00, 0x00, 0x00}
#define X_PPM (uint8_t[]){0x00, 0x10, 0000, 0x00}
#define Y_PPM (uint8_t[]){0x00, 0x10, 0x00, 0x00}
#define COLORS (uint8_t[]){0x00, 0x00, 0x00, 0x00}
#define IMPORTANT_COLORS (uint8_t[]){0x00, 0x00, 0x00, 0x00}

void encode(uint8_t *image, uint32_t width, uint32_t height, const char * const name)
{

    FILE *bmp = fopen(name, "w");

    int paddingBytes = width % 4;

    uint64_t imageSize = (width * height * 3) + (paddingBytes * height);
    uint8_t *pixels = malloc(imageSize);
    uint32_t fileSize = HEADER_SIZE + INFO_HEADER_SIZE + (width * height * 3) + (height * paddingBytes);

    int i = 0;

    for(int y = 0; y < height; y++)
    {
        int pos = y * ((width * 3) + paddingBytes);
        
        for(int x = 0; x < width; x++)
        {
            pixels[pos + (x * 3)] = image[i];
            pixels[pos + (x * 3) + 1] = image[i + 1];
            pixels[pos + (x * 3) + 2] = image[i + 2];
            
            i += 3;

            if(y == height / 2 || x == width / 2)
            {
                pixels[pos + (x * 3)] = 0;
                pixels[pos + (x * 3) + 1] = 0;
                pixels[pos + (x * 3) + 2] = 0;
            }
        }
        
        for(int b = 0; b < paddingBytes; b++)
        {
            pixels[pos + b] = 0x00;

            i++;
        }
    }
    
    fwrite(SIGNATURE, sizeof(uint8_t), sizeof(SIGNATURE), bmp);
    fputc(fileSize & 0x000000ff, bmp);
    fputc((fileSize >> 8) & 0x000000ff, bmp);
    fputc((fileSize >> 16) & 0x000000ff, bmp);
    fputc((fileSize >> 24) & 0x000000ff, bmp);
    fwrite(RESERVED, sizeof(uint8_t), sizeof(RESERVED), bmp);
    fputc(DATA_OFFSET & 0x000000ff, bmp);
    fputc((DATA_OFFSET >> 8) & 0x000000ff, bmp);
    fputc((DATA_OFFSET >> 24) & 0x000000ff, bmp);
    fputc((DATA_OFFSET >> 16) & 0x000000ff, bmp);

    fputc(INFO_HEADER_SIZE & 0x000000ff, bmp);
    fputc((INFO_HEADER_SIZE >> 8) & 0x000000ff, bmp);
    fputc((INFO_HEADER_SIZE >> 24) & 0x000000ff, bmp);
    fputc((INFO_HEADER_SIZE >> 16) & 0x000000ff, bmp);
    fputc(width & 0x000000ff, bmp);
    fputc((width >> 8) & 0x000000ff, bmp);
    fputc((width >> 16) & 0x000000ff, bmp);
    fputc((width >> 24) & 0x000000ff, bmp);
    fputc(height & 0x000000ff, bmp);
    fputc((height >> 8) & 0x000000ff, bmp);
    fputc((height >> 16) & 0x000000ff, bmp);
    fputc((height >> 24) & 0x000000ff, bmp);

    fwrite(PLANES, sizeof(uint8_t), sizeof(PLANES), bmp);
    fwrite(BITS_PER_PIXEL, sizeof(uint8_t), sizeof(BITS_PER_PIXEL), bmp);
    fwrite(COMPRESSION, sizeof(uint8_t), sizeof(COMPRESSION), bmp);
    fwrite(COMPRESSED_SIZE, sizeof(uint8_t), sizeof(COMPRESSED_SIZE), bmp);
    fwrite(X_PPM, sizeof(uint8_t), sizeof(X_PPM), bmp);
    fwrite(Y_PPM, sizeof(uint8_t), sizeof(Y_PPM), bmp);
    fwrite(Y_PPM, sizeof(uint8_t), sizeof(Y_PPM), bmp);
    fwrite(COLORS, sizeof(uint8_t), sizeof(COLORS), bmp);
    fwrite(IMPORTANT_COLORS, sizeof(uint8_t), sizeof(IMPORTANT_COLORS), bmp);

    fwrite(pixels, sizeof(uint8_t), imageSize, bmp);

    fclose(bmp);

    free(pixels);
    free(image);
}
