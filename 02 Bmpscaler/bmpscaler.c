// Resizes a BMP file, argv[1] - float from 0.0 to 100, argv[2] - source file name (*.bmp) , argv[3] - new resized file name (*.bmp)
// E.g. $ ./bmpscaler 0.3 picture.bmp new.bmp
// Implemented even distribution algorithm with controlling fix

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

void argumentChecks(int argc, char *argv[], int *errorFlagPtr);
void openFiles(char inFileName[], char outFileName[], FILE **inptr, FILE **outptr, int *errorFlagPtr);
void checkIf24Bit(BITMAPFILEHEADER *bf, BITMAPINFOHEADER *bi, FILE **inptr, FILE **outptr, int *errorFlagPtr);
void updateHeader(BITMAPFILEHEADER *bf, BITMAPINFOHEADER *bi, int *outpadding, double scale, int biHeightsign);
void distributeCopies(int container[],int valueOrig, int valueNew);
void write(int *counter, int limit, RGBTRIPLE *inptr, FILE *outptr);

int main(int argc, char *argv[])
{
    int errorFlag = 0;
    argumentChecks(argc, argv, &errorFlag);
    if(errorFlag) return errorFlag; // Stop main if errors found by argumentChecks

    // Convert argv[1] to double, creating resize multiplier
    double scale = atof(argv[1]);
    printf("scaling factor = %f\n", scale);

    // remember filenames
    char *inFileName = argv[2];
    char *outFileName = argv[3];

    // Prepare file pointers for source and new file
    FILE *inptr = NULL;
    FILE *outptr = NULL;

    openFiles(inFileName, outFileName, &inptr, &outptr, &errorFlag); // Open files in r and w mode
    if(errorFlag) return errorFlag; // Stop main if errors found by openFiles

    // read Source file's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read Source file's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    checkIf24Bit(&bf, &bi, &inptr, &outptr, &errorFlag);  // ensure Source file is a 24-bit uncompressed BMP 4.0
    if(errorFlag) return errorFlag; // Stop main if errors found by checkIf24Bit

    // Debug output
    printf("bfSize = %i\n", bf.bfSize);
    printf("biWidth = %i\n", bi.biWidth);
    printf("biHeight = %i\n", bi.biHeight);
    printf("biSizeImage = %i\n", bi.biSizeImage);

    // Store source file header parameters
    int inpadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;  // Remember original padding
    printf("inpadding = %i\n", inpadding);
    int widthSource = bi.biWidth; // Remember original RGB table width
    int heightSource = abs(bi.biHeight); // Remember original RGB table height
    int biHeightsign = 1;  // Remember if bi.biHeight is pos or neg
    if (bi.biHeight < 0)
    {
        biHeightsign = -1;
    }

    // Calculate new header parameters
    int outpadding; // Padding for output file scanlines
    updateHeader(&bf, &bi, &outpadding, scale, biHeightsign); // Updates header parameters

    // Display data for debug purpose
    printf("bfSize = %i\n", bf.bfSize);
    printf("biWidth = %i\n", bi.biWidth);
    printf("biHeight = %i\n", bi.biHeight);
    printf("biSizeImage = %i\n", bi.biSizeImage);
    printf("outpadding = %i\n", outpadding);

    // write resized file's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write resized file's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Prepare array to store no of copies to me made of each original scanline
    int linecopy[heightSource];
    for (int i = 0; i < heightSource; i++)
    {
        linecopy[i] = 0;
    }

    int heightNew = abs(bi.biHeight); // New bmp file vertical size
    printf("heightNew = %i\n", heightNew); // debug data

    distributeCopies(linecopy, heightSource, heightNew);

    // Prepare array to store no of copies to be made of each source pixel in scanline
    int pixcopy[widthSource];
    for (int i = 0; i < widthSource; i++)
    {
        pixcopy[i] = 0;
    }

    distributeCopies(pixcopy, widthSource, bi.biWidth); // By this point bi.biWidth was updated by updateHeader()

    int count = 0; // Keeping track of pixels per line not to exceed bi.biWidth.
    int persistentcount = 0; // Persistent pixel counter for displaying pixels printed by algorythm
    int persistentcountfix = 0; // Persistent pixel counter for displaying total printed pixels after fix was applied
    int linecount = 0; // Keeping track of total lines printed.
    int linecountprefix = 0; // Keeping track of lines printed before fix applied.

    // iterate over Source file's scanlines
    for (int iv = 0; iv < heightSource; iv++)
    {
        if (linecopy[iv] == 0)
        {
            fseek(inptr, (widthSource * sizeof(RGBTRIPLE)) + inpadding, SEEK_CUR);
        }

        for (int jv = 0; jv < linecopy[iv]; jv++)
        {
            if (linecount < heightNew)
            {
                RGBTRIPLE triple; // temporary storage

                // iterate over pixels in scanline
                for (int i = 0; i < widthSource; i++)
                {
                    // read RGB triple from Source file
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                    for (int j = 0; j < pixcopy[i]; j++)
                    {
                        write(&count, bi.biWidth, &triple, outptr);
                    }
                }
                // printf("line pixels counted: %i\n", count);
                persistentcount = count;

                // duplicate last pixel if algorithm failed
                while (count < bi.biWidth)
                {
                    write(&count, bi.biWidth, &triple, outptr);
                }

                // Reset pixel counter at line end
                persistentcountfix = count;
                count = 0;

                // Scroll back to beginnig of the line
                fseek(inptr, widthSource * -1 * sizeof(RGBTRIPLE), SEEK_CUR);

                // Add output file padding if required
                for (int i = 0; i < outpadding; i++)
                {
                    fputc(0x00, outptr);
                }

                linecount++;

                linecountprefix = linecount;

                // Detect when new sorce line has to be scanned
                if (jv + 1 == linecopy[iv])
                {
                    // Scroll to end of the line
                    fseek(inptr, (widthSource * sizeof(RGBTRIPLE)) + inpadding, SEEK_CUR);
                }
            }
        }
    }

    // Duplicate last line if algorithm failed
    while (linecount < heightNew)
    {
        // temporary storage
        RGBTRIPLE triple;

        // Scroll back to beginnig of the line
        fseek(inptr, (widthSource * -1 * sizeof(RGBTRIPLE)) - inpadding, SEEK_CUR);

        // iterate over pixels in scanline
        for (int i = 0; i < widthSource; i++)
        {
            // read RGB triple from Source file
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            for (int j = 0; j < pixcopy[i]; j++)
            {
                write(&count, bi.biWidth, &triple, outptr);
            }
        }
        // printf("line pixels counted: %i\n", count);
        persistentcount = count;

        // duplicate last pixel if algorithm failed
        while (count < bi.biWidth)
        {
            write(&count, bi.biWidth, &triple, outptr);
        }

        // Reset pixel counter at line end
        persistentcountfix = count;
        count = 0;

        // Scroll over input file padding
        fseek(inptr, inpadding, SEEK_CUR);

        // Add output file padding if required
        for (int i = 0; i < outpadding; i++)
        {
            fputc(0x00, outptr);
        }

        linecount++;
    }

    for (int i = 0; i < widthSource; i++)
    {
        printf("%i'th pixel copies: %i\n", i + 1, pixcopy[i]);
    }
    for (int i = 0; i < heightSource; i++)
    {
        printf("%i'th line copies: %i\n", i + 1, linecopy[i]);
    }

    // Debug data output
    printf("pixels printed by algorithm = %i\n", persistentcount);
    printf("pixel printed after fix = %i\n", persistentcountfix);
    printf("pixels printed by algorithm = %i\n", linecountprefix);
    printf("total printed lines = %i\n", linecount);
    printf("size of RGBTRIPLE is %i\n", (int)sizeof(RGBTRIPLE));

    fclose(inptr);  // close Source file
    fclose(outptr);  // close resized file
    return 0;  // success
}

// write RGB triple to resized file
void write(int *counter, int limit, RGBTRIPLE *inptr, FILE *outptr)
{
    if (*counter < limit)
    {
        fwrite(inptr, sizeof(RGBTRIPLE), 1, outptr);
        *counter = *counter + 1;
    }
}

// Conducts initial argument input checks
void argumentChecks(int argc, char *argv[], int *errorFlagPtr)
{
    if (argc != 4)  // Check number of arguments
    {
        printf("Wrong number of arguments\n");
        *errorFlagPtr = 1;
        return;
    }

    // Check to see if only digits or decimal were typed
    for (int i = 0, n = strlen(argv[1]); i < n; i++)
    {
        if (argv[1][i] < 46 || argv[1][i] > 57 || argv[1][i] == 47) // If within ASCII digit range
        {
            printf("Scaling multiplier error, please input digits only");
            *errorFlagPtr = 1;
            return;
        }
    }
    // Checking of resize multiplier range
    double scale = atof(argv[1]); // Convert argv[1] to double
    if (scale <= 0 || scale > 100)
    {
        printf("Scaling multiplier error, please input positive value from 0.0 to 100\n");
        *errorFlagPtr = 1;
        return;
    }
}

void openFiles(char inFileName[], char outFileName[], FILE **inptr, FILE **outptr, int *errorFlagPtr)
{
    *inptr = fopen(inFileName, "r"); // Opens in reading mode source bmp file
    if (!*inptr)
    {
        printf("Could not open %s.\n", inFileName);
        *errorFlagPtr = 2;
        return;
    }

    *outptr = fopen(outFileName, "w"); // Creates and opens in writing mode new bmp file
    if (!*outptr)
    {
        fclose(*inptr);
        printf("Could not create %s.\n", outFileName);
        *errorFlagPtr = 3;
        return;
    }
}

// Ensures Source file is (likely) a 24-bit uncompressed BMP 4.0
void checkIf24Bit(BITMAPFILEHEADER *bf, BITMAPINFOHEADER *bi, FILE **inptr, FILE **outptr, int *errorFlagPtr)
{
    // Checks key header parameters
    if (bf->bfType != 0x4d42 || bf->bfOffBits != 54 || bi->biSize != 40 ||
        bi->biBitCount != 24 || bi->biCompression != 0)
    {
        fclose(*outptr);
        fclose(*inptr);
        printf("Unsupported file format.\n");
        *errorFlagPtr = 4;
        return;
    }
}

// Updates header parameters for new file
void updateHeader(BITMAPFILEHEADER *bf, BITMAPINFOHEADER *bi, int *outpadding, double scale, int biHeightsign)
{
    bi->biWidth = (LONG)(((double)bi->biWidth) * scale); // Update RGB table width
    if (bi->biWidth < 1)
    {
        bi->biWidth = 1;
    }

    bi->biHeight = (LONG)(((double)bi->biHeight) * scale);
    if (abs(bi->biHeight) < 1)
    {
        bi->biHeight = 1 * biHeightsign;
    }

    *outpadding = (4 - (bi->biWidth * sizeof(RGBTRIPLE)) % 4) % 4; // Padding for output file scanlines
    bi->biSizeImage = (DWORD)(((bi->biWidth * sizeof(RGBTRIPLE)) + *outpadding) * abs(bi->biHeight));
    bf->bfSize = bi->biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
}

// Calculates how many copies of each original object to make to get new total item count
void distributeCopies(int container[],int valueOrig, int valueNew)
{
    // Add floor(valueNew/valueOrig) copies for each original pixel.
    for (int i = 0; i < valueOrig; i++)
    {
        for (int j = 0, n = valueNew / valueOrig; j < n; j++)
        {
            container[i]++;
        }
    }

    // Distributes remaining items among container[] cells
    if ((valueNew % valueOrig) == 0)  // If valueNew % original == 0, don't do anything
    {}

    else if ((valueOrig / (valueNew % valueOrig)) >= 2) // If k1 >= 2 add extra copy of every kth item
    {
        int k1 = (valueOrig / (valueNew % valueOrig));
        printf("k1 = %i\n", k1);
        for (int i = 0; i < valueOrig; i++)
        {
            if ((i + 1) % k1 == 0)
            {
                container[i]++;
            }
        }
    }

    else  // If k1 < 2 add extra copy to every original item except kth items
    {
        int k2 = (valueOrig / (valueOrig - (valueNew % valueOrig)));
        printf("k2 = %i\n", k2);
        for (int i = 0; i < valueOrig; i++)
        {
            if ((i + 1) % k2 != 0)
            {
                container[i]++;
            }
        }
    }
}