#pragma once
#ifndef __BMP_H__
#define __BMP_H__

#include <stdint.h>

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

typedef struct
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    WORD   bfOffBits;
}__attribute__((__packed__)) BITMAPFILEHEADER;

typedef struct
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} __attribute__((__packed__)) BITMAPINFOHEADER;

typedef struct
{
    BYTE  rgbtBlue;
    BYTE  rgbtGreen;
    BYTE  rgbtRed;
} __attribute__((__packed__)) RGBTRIPLE;

typedef struct
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    RGBTRIPLE **image;
}__attribute__((__packed__)) BMP;

void printBMP(const BMP bmp);
void printBMPInfo(const BMP bmp);

#endif