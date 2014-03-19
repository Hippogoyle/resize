/**
 * by Jerry Johnson
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];
    
    //check n for positive int from 1 to 100
    int n = atoi(argv[1]);
    if (n < 1 || n > 100)
    {   
        printf("n value must be between 1 and 100\n");
        return 1;
    }   

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
     // save input struct values
    int inputWidth = bi.biWidth;
    int inputHeight = bi.biHeight;
    
  //  int inputISize = bi.biSizeImage;
    
    // resize  biWidth and biHeight in pixels
    bi.biWidth *= n;
    bi.biHeight *= n;
    
    // determine padding for scanlines
    int inputpadding  = (4 - ((inputWidth) * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding = (4 - ((bi.biWidth) * sizeof(RGBTRIPLE)) % 4) % 4;
    
    //char* scanline [bi.biWidth * 3 + padding];
    // resize the total size of image + padding
    bi.biSizeImage = abs(bi.biHeight) * ((bi.biWidth * 3) + padding);
    
    bf.bfSize = (54 + bi.biSizeImage);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);
 
    // iterate over infile's scanlines
    RGBTRIPLE* buffer = malloc(sizeof(RGBTRIPLE) * bi.biWidth);
    for (int i = 0; i < abs(inputHeight); i++)
    {   
    // iterate over pixels in scanline
        int g = 0;
        for (int j = 0; j < inputWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            for (int m = 0; m < n; m++, g++)
            {
                buffer[g] = triple;  
            }
        }

        // skip over padding, if any
        fseek(inptr, inputpadding, SEEK_CUR);
        
        for(int size =0; size < n; size++)
        {
            fwrite(buffer, sizeof(RGBTRIPLE), bi.biWidth, outptr);
        }

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);   
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    
    free(buffer);

    // that's all folks
    
    return 0;
}
