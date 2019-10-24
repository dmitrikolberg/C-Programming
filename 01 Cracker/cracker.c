// Cracks hash string generated by crypt() function.
// Use hash string as an argument.
// E.g.: $ ./cracker 50cI2vYkF0YU2

#include <stdio.h>
#include <crypt.h>
#include <string.h>

#define iterationLimit 500000000

void rotate(int ltr_num, char kword[]);

int main(int argc, char* argv[])
{
    // Check to see that only 1 argument was input
    if (argc != 2)
    {
        printf("Usage: ./cracker hash\n");
        return 1;
    }

    // Initialize password string
    char c1 = 'A';
    char c2 = '\0';
    char c3 = '\0';
    char c4 = '\0';
    char c5 = '\0';
    char c6 = '\0';
    char pwdout[] = {c1, c2, c3, c4, c5, c6, '\0'};

    // Salt calculation
    char salt[] = {argv[1][0], argv[1][1], '\0'};

    // Probe for password
    for (unsigned int i = 0; i < iterationLimit; i++)
    {
        // Enrypt current password sample
        char *hashout = crypt(pwdout, salt);

        if (strcmp(argv[1], hashout) == 0)
        {
            // Match found
            printf("%s\n", pwdout);
            return 0;
        }
        else
        {
            rotate(0, pwdout);
        }
    }

    printf("match not found - loop limit reached\n");
    return 1;
}

// Rotates first password letter by one and after full cycle (when it was'z')
// resets to 'A' and changes next letter(s) by 1
void rotate(int ltr_num, char kword[])
{
    kword[ltr_num] = kword[ltr_num] + 1;

    if (kword[ltr_num] == '[') // Ascii '[' == 91, follows ascii 'Z' == 90
    {
        kword[ltr_num] = 'a'; // Ascii 'a' == 97
    }
    if (kword[ltr_num] == '{') // Ascii '{' == 123, follows ascii 'z' == 122
    {
        kword[ltr_num] = 'A'; // Ascii 'A' == 65
        rotate(ltr_num + 1, kword);

        // When new letter is initialized, its ascii value of 0 ('\0') changes to 1 (Start Of Heading)
        // and must be scrolled to 'A'
        if (kword[ltr_num + 1] == 1) // Ascii "Start of Heading"
        {
            kword[ltr_num + 1] = 'A'; // Ascii 'A' == 65
        }
    }
}
