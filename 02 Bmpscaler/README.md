### BMPSCALER

#### Short description:
"bmpscaler.c" creates a rezized copy of 24-bit BMP image. 

#### Specification:
•	Program accepts exactly three command-line arguments, where:<br/>
•	the first must be a floating-point value in the range (0.0, 100.0] (ie., a positive value less than or equal to 100.0),<br/>
•	the second must be the name of a BMP to be resized, and<br/>
•	the third must be the name of the resized version to be written.<br/>
•	If program is not executed with such, it reminds the user of correct usage, and main returns 1.<br/>

#### Example behavior:
$ ./bmpscaler<br/>
Wrong number of arguments<br/>
$ echo $?<br/>
1<br/>
<br/>
$ ./bmpscaler 0.5 picture.bmp smaller.bmp<br/>
$ echo $?<br/>
0<br/>
<br/>
$ ./bmpscaler 3 picture.bmp new.bmp<br/>
$ echo $?<br/>
0<br/>

#### Warning:
"Bmpscaler.c" was created and intended to run on Linux system.<br/>

#### Extras:
Header file "bmp.h" containing BMP related structure types is included.