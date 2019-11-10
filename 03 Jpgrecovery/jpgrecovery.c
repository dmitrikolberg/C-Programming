// Recovers JPEGs from a forensic image
// Use name of source file with JPEGs as an argument
// E.g.: $ ./jpgrecovery card.raw

#include <stdio.h>
#include <stdlib.h>

#define blockSize 512

int fourBytesAreJpg(unsigned char buffer[]);
void writeToNew(char filename[], unsigned char buffer[], FILE **outPointer, int *errorFlagPtr);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("wrong argument count\n");
        return 1;
    }

    FILE *image = fopen(argv[1], "r"); // Open source image file
    if (!image)
    {
        printf("Error reading file\n");
        return 2;
    }

    unsigned char buffer[blockSize + 1] = {'\0'}; // Buffer to store data
    int jpgcounter = 0;
    int firstJpgFound = 0;
    char filename[] = "000.jpg";
    int errorFlag = 0;
    FILE *output = NULL; // File pointer to JPEG being processed

    while (fread(buffer, blockSize, 1, image)) // Repeat till read returns 0 (EOF reached)
    {
        if (fourBytesAreJpg(buffer)) // If jpg start signature is detected in a buffer
        {
            if (firstJpgFound == 0) // If no jpg files were encountered before
            {
                firstJpgFound = 1;
                writeToNew(filename, buffer, &output, &errorFlag);
                if (errorFlag != 0) // If writeToNew failed then stop main
                {
                    return errorFlag;
                }
            }
            else // If jpg files were encountered before
            {
                fclose(output); // Finalize currently opened file
                jpgcounter++;
                sprintf(filename, "%03i.jpg", jpgcounter); // Update filename for next Jpg file
                writeToNew(filename, buffer, &output, &errorFlag);
                if (errorFlag != 0) // If writeToNew failed then stop main
                {
                    return errorFlag;
                }
            }
        }
        else // If jpg start signature is not detected in a buffer
        {
            if (firstJpgFound != 0) // If jpg files were encountered before
            {
                fwrite(buffer, blockSize, 1, output); // Copy data from buffer to currently opened file
            }
            else // If no jpg files were encountered before then do nothing
            {}
        }
    }
    fclose(image);
    if (output) // Finalize last opened jpg file if any were detected in a source image
    {
        fclose(output);
    }
    printf("recovery complete\n");
}

int fourBytesAreJpg(unsigned char buffer[]) // Checks if first four bytes are JPEG signature
{
    return buffer[0] == 0xff &&
    buffer[1] == 0xd8 &&
    buffer[2] == 0xff &&
    buffer[3] > 0xdf &&
    buffer[3] < 0xf0;
}

void writeToNew(char filename[], unsigned char buffer[], FILE **outPointer, int *errorFlagPtr)
{
    *outPointer = fopen(filename, "w"); // Creates and opens in writing mode new jpg file
    if (!*outPointer)
    {
        printf("Error writing jpg file\n");
        *errorFlagPtr = 3;
        return;
    }
    fwrite(buffer, blockSize, 1, *outPointer); // Copies data from buffer to opened file
}

