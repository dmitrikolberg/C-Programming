// Cracks hash string generated by crypt() function.
// Use hash string as an argument.
// E.g.: $ ./cracker 50cI2vYkF0YU2

#include <stdio.h>
#include <crypt.h>
#include <string.h>

#define maxPassLength 5 // Set expected max password length here

void setNextAlphabetic(int charIndex, char text[]);

int main(int argc, char* argv[])
{
    // Check to see that only 1 argument was input
    if (argc != 2)
    {
        printf("Usage: ./cracker hash\n");
        return 1;
    }

    // Initialize password
    char password[maxPassLength + 1] = {'\0'};
    password[0] = 'A';

    // Salt initialization
    char salt[] = {argv[1][0], argv[1][1], '\0'};

    // Check if password matches
    while (password[maxPassLength] == '\0')
    {
        // Enrypt current password sample
        char *hashout = crypt(password, salt);

        // Compare hash strings
        if (strcmp(argv[1], hashout) == 0)
        {
            // Match found
            printf("%s\n", password);
            return 0;
        }
        else
        {
            setNextAlphabetic(0, password);
        }
    }

    printf("match not found - loop limit reached\n");
    return 2;
}

// Rotates first letter by one and after full cycle (when it was'z')
// resets to 'A' and changes next letter(s) by 1
void setNextAlphabetic(int charIndex, char text[])
{
    text[charIndex] = text[charIndex] + 1;

    if (text[charIndex] == '[') // When out of uppercase Ascii range; Ascii '[' follows ascii 'Z'.
    {
        text[charIndex] = 'a'; // Set to beginning of lowercase Ascii range.
    }
    else if (text[charIndex] == '{') // When out of lowercase Ascii range; Ascii '{' follows ascii 'z'.
    {
        text[charIndex] = 'A'; // Set to beginning of uppercase Ascii range. Restarts character rotation cycle.

        // At this point character rotation cycle was reset, and we have to change next letter.
        setNextAlphabetic(charIndex + 1, text); // Rotates next letter(s)

        // When new letter is initialized, its ascii value of 0 ('\0') changes to 1 ("Start Of Heading")
        // and must be scrolled to 'A'
        if (text[charIndex + 1] == 1) // When new letter was initialized; Ascii "Start of Heading" == 1
        {
            text[charIndex + 1] = 'A'; // Set to beginning of uppercase Ascii range.
        }
    }
}





