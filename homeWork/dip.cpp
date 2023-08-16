#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
typedef unsigned char BYTE;   // 1 byte of memory
typedef unsigned short WORD;  // 2 bytes of memory
typedef unsigned int DWORD;   // 4 bytes of memory
typedef int LONG;             // 4 bytes of memory

typedef struct _BMPFH
{
    BYTE bftype1;
    BYTE bftype2;
    DWORD bfsize;
    WORD bfreserved1;
    WORD bfreserved2;
    DWORD bfOffbits;
} BMPFH;

typedef struct _BMPIH
{
    DWORD bisize;
    LONG biw;
    LONG bih;
    WORD biplane;
    WORD bibitcount;
    DWORD biComp;
    DWORD bisizeimage;
    LONG bix;
    LONG biy;
    DWORD biclused;
    DWORD biclimp;
} BMPIH;

typedef struct _PALET
{
    BYTE rgbblue;
    BYTE rgbgreen;
    BYTE rgbred;
    BYTE rgbreserved;
} PALET;

typedef struct _IMAGE
{
    BMPFH bmpfh;
    BMPIH bmpih;
    PALET *palet;
    BYTE *data;
} IMAGE;

IMAGE *ImageRead(IMAGE *image, char *filename)
{
    BMPFH bmpfh;
    BMPIH bmpih;
    FILE *fp;
    DWORD r, rowsize, size;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("File is not found..");
        exit(1);
    }
    fread(&bmpfh, sizeof(BMPFH), 1, fp);
    if (bmpfh.bftype1 != 'B' || bmpfh.bftype2 != 'M')
    {
        printf("This is not a bitmap file.");
        exit(1);
    }
    fread(&bmpih, sizeof(BMPIH), 1, fp);
    image = (IMAGE *)malloc(sizeof(IMAGE));
    if (image == NULL)
    {
        printf("There is no enough memory for this operation");
        exit(1);
    }
    image->bmpfh = bmpfh;
    image->bmpih = bmpih;

    r = 0;
    if (bmpih.bibitcount == 1)
        r = 2;
    if (bmpih.bibitcount == 4)
        r = 16;
    if (bmpih.bibitcount == 8)
        r = 256;
    if (r != 0)
    {
        image->palet = (PALET *)malloc(sizeof(PALET) * r);
        fread(image->palet, sizeof(PALET), r, fp);
    }
    rowsize = (image->bmpih.biw * image->bmpih.bibitcount + 31) / 32 * 4;
    size = rowsize * image->bmpih.bih;
    image->data = (BYTE *)malloc(size);
    fread(image->data, size, 1, fp);
    fclose(fp);
    return image;
}

void ImageWrite(IMAGE *image, char *filename)
{
    FILE *fp;
    int r, rowsize, size;

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("File opening error!");
        exit(1);
    }
    fwrite(&image->bmpfh, sizeof(BMPFH), 1, fp);
    fwrite(&image->bmpih, sizeof(BMPIH), 1, fp);
    r = 0;
    if (image->bmpih.bibitcount == 1)
        r = 2;
    if (image->bmpih.bibitcount == 4)
        r = 16;
    if (image->bmpih.bibitcount == 8)
        r = 256;
    if (r != 0)
        fwrite(image->palet, sizeof(PALET), r, fp);
    rowsize = (image->bmpih.biw * image->bmpih.bibitcount + 31) / 32 * 4;
    size = rowsize * image->bmpih.bih;
    fwrite(image->data, size, 1, fp);
    fclose(fp);
}

void writeInfo(IMAGE *image, char *fname)
{
    printf("--------Info about %s  image file\n", fname);
    printf("bfType value\t\t:%c%c\n", image->bmpfh.bftype1, image->bmpfh.bftype2);
    printf("bfsize\t\t\t:%d\n", image->bmpfh.bfsize);
    printf("bfreserved1\t\t:%d\n", image->bmpfh.bfreserved1);
    printf("bfreserved2\t\t:%d\n", image->bmpfh.bfreserved2);
    printf("bfOffbits\t\t:%d\n", image->bmpfh.bfOffbits);
    printf("bisize\t\t\t:%d\n", image->bmpih.bisize);
    printf("Width\t\t\t:%d\n", image->bmpih.biw);
    printf("Height\t\t\t:%d\n", image->bmpih.bih);
    printf("biplane\t\t\t:%d\n", image->bmpih.biplane);
    printf("bibitcount\t\t:%d\n", image->bmpih.bibitcount);
    printf("Compression\t\t:%d\n", image->bmpih.biComp);
    printf("bisizeimage\t\t:%d\n", image->bmpih.bisizeimage);
    printf("bix\t\t\t:%d\n", image->bmpih.bix);
    printf("biy\t\t\t:%d\n", image->bmpih.biy);
    printf("bi color used\t\t:%d\n", image->bmpih.biclused);
    printf("bi color imp.\t\t:%d\n", image->bmpih.biclimp);
}

void setZero(IMAGE *image)
{
    int i, rowsize;
    rowsize = (image->bmpih.biw * image->bmpih.bibitcount + 31) / 32 * 4;
    for (i = 0; i < image->bmpih.bih * rowsize; i++)
        image->data[i] = 0;
}

int findNearestColorIndex(PALET *palette, BYTE r, BYTE g, BYTE b)
{
    int nearestIndex = 0;
    int minDistance = 255 * 255 * 3; // Baþlangýçta en büyük mesafeyi ayarlayýn
    
    for (int i = 0; i < 256; i++) {
        // Renk paletindeki renk deðerleri
        BYTE pr = palette[i].rgbred;
        BYTE pg = palette[i].rgbgreen;
        BYTE pb = palette[i].rgbblue;
        
        // Renkler arasýndaki mesafeyi hesapla
        int distance = (r - pr) * (r - pr) + (g - pg) * (g - pg) + (b - pb) * (b - pb);
        
        // En küçük mesafeyi güncelle
        if (distance < minDistance) {
            minDistance = distance;
            nearestIndex = i;
        }
    }
    
    return nearestIndex;
}

void convertTo8BitGray(IMAGE *image)
{
    int i, j, rowsize;
    rowsize = (image->bmpih.biw + 3) / 4 * 4; // Satýr boyutunu 4'e tamamlayacak þekilde ayarlama

    BYTE *newData = (BYTE *)malloc(image->bmpih.bih * rowsize);
    if (newData == NULL) {
        printf("Memory allocation error.");
        exit(1);
    }

    for (i = 0; i < image->bmpih.bih; i++) {
        for (j = 0; j < image->bmpih.biw; j++) {
            BYTE r = image->data[i * rowsize * 3 + j * 3 + 2];
            BYTE g = image->data[i * rowsize * 3 + j * 3 + 1];
            BYTE b = image->data[i * rowsize * 3 + j * 3];

            // RGB renk deðerlerini griye dönüþtürme
            BYTE gray = (BYTE)((r + g + b) / 3);

            // Yeni piksel verilerine atama
            newData[i * rowsize + j] = gray;
        }
    }

    free(image->data);
    image->data = newData;
    image->bmpih.bibitcount = 8;

    // Yeni palet oluþturma
    free(image->palet);
    image->palet = (PALET *)malloc(sizeof(PALET) * 256);
    for (i = 0; i < 256; i++) {
        image->palet[i].rgbblue = i;
        image->palet[i].rgbgreen = i;
        image->palet[i].rgbred = i;
        image->palet[i].rgbreserved = 0;
    }

    // Görüntü boyutunu güncelleme
    image->bmpih.bisizeimage = image->bmpih.bih * rowsize;
}

int main()
{
    IMAGE *image = NULL;
    image = ImageRead(image, "parrots.bmp");
    

    convertTo8BitGray(image);

    ImageWrite(image, "parrots_gray.bmp");
    free(image->palet);
    free(image->data);
    free(image);

    return 0;
}
