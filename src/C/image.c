#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

BMP init(const char *path)
{
    BMP bmp;

    FILE *bmpFile = fopen(path, "rb");

    if (!bmpFile)
    {
        fprintf(stderr, "Error, opening BMP file!\n");
        bmp.size = -1;
        exit(EXIT_FAILURE);
    }

    fread(&bmp.fileHeader, sizeof(BITMAPFILEHEADER), 1, bmpFile);
    fread(&bmp.infoHeader, sizeof(BITMAPINFOHEADER), 1, bmpFile);

    int dataSize = bmp.infoHeader.biSizeImage;
    bmp.pixelData = malloc(dataSize);

    if (!bmp.pixelData)
    {
        fprintf(stderr, "Memory allocation failed!");
        bmp.size = -1;
        fclose(bmpFile);
    }

    fread(bmp.pixelData, sizeof(BYTE), dataSize, bmpFile);

    fclose(bmpFile);

    return bmp;
}

void main(int argc, char *argv[])
{   
    BMP bmp;

    bmp = init(argv[1]);

    printf("%i", bmp.size);

    exit(EXIT_SUCCESS);
}