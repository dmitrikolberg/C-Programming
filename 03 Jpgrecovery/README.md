### JPGRECOVERY

#### Short description:
"jpgrecovery.c" is a program that recovers JPEGs from a forensic image (card.raw file storing byte after byte memory card data, formatted with FAT system with "block size" of 512 bytes).

#### Specification:
•	Program accepts exactly one command-line argument, the name of a forensic image from which to recover JPEGs.<br/> 
• 	If program is not executed with exactly one command-line argument, it reminds the user of correct usage, and main returns 1.<br/>
•	If the forensic image cannot be opened for reading, program informs the user as much, and main returns 2.<br/>

#### Example behavior:
$ ./jpgrecovery<br/>
wrong argument count<br/>
$ echo $?<br/>
1<br/>

$ ./jpgrecovery card.raw<br/>
$ echo $?<br/>
0<br/>

#### Warning:
"jpgrecovery.c" was created and intended to run on Linux system.<br/>

#### Extras:
Forensic image "card.raw" containing 50 JPEG images is included.<br/>