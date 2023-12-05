#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

BMP *initBMP(const char *path)
{

    FILE *inptr = fopen(path, "r");

    if (inptr == NULL)
    {
        printf("Could not open %s.\n", path);
        return NULL;
    }

    BMP *bmp;

    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    /* Check if the file is a 24-bit uncompressed BMP 4.0

    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || bi.biBitCount != 24 || bi.biCompression != 0)
    {
        printf("Unsupported file format!\n");
        fclose(inptr);

        return NULL;
    }*/

    LONG height = abs(bi.biHeight);
    LONG width = bi.biWidth;

    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(inptr);

        return NULL;
    }

    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    for (int i = 0; i < height; i++)
    {
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);
        fseek(inptr, padding, SEEK_CUR);
    }

    fclose(inptr);

    return bmp;
}

void printBMPInfo(const BMP *bmp)
{
    printf("%d\n", bmp->infoHeader.biWidth);
    printf("%d\n", bmp->infoHeader.biHeight);
}

void printBMP(const BMP *bmp)
{
    LONG height = abs(bmp->infoHeader.biHeight);
    LONG width = bmp->infoHeader.biWidth;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("R:%u G:%u B:%u ", bmp->pixelData[i][j].rgbtRed, bmp->pixelData[i][j].rgbtGreen, bmp->pixelData[i][j].rgbtBlue);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <BMP file path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    BMP *bmp = initBMP(argv[1]);

    if (bmp == NULL)
    {
        printf("Failed!\n");
        return EXIT_FAILURE;
    }

    if (bmp->infoHeader.biSize < 0)
    {
        printf("Failed to initialize BMP!\n");
        return EXIT_FAILURE;
    }

    printf("Size of the BMP file: %d\n\n", bmp->infoHeader.biSize);

    printBMPInfo(bmp);
    printBMP(bmp);

    free(bmp->pixelData);

    return EXIT_SUCCESS;
}