# File Assignment
### Contributors: Kalash Shah and Prajas Naik

### Usage: 

Please enter the following commands in bash
```Bash
    chmod +x error_handling.sh
    make
    ./flame_cp #-i <source file pathname> -o <destination file pathname>
    ./error_handling.sh $?
```
Please note, the `./error_handling.sh $?` should be used immediately after the flame_cp program is executed.

### Behavior:

Copies from input file to output file (or standard I/O). Please note, the input file will be copied to the output file, replacing any content that was present. If input file content is less than output file content, only the number of bytes equal to size of the input file will be modified, the other bytes will remain as is. 

### Implementation: 

This program makes use of the `read()` and `write()` system calls to copy from input file to output file. While the `sendfile()` system call is more efficient, we implemented using `read()` and `write()` for learning purposes. Any errors in system calls are returned as positive int error codes as defined in `errno.h`. Any custom error codes as defined by us are also returned appropriately as positive integers. 

All error codes are at max 3 digit numbers. Hence multiple error codes are handled by multiplying the previous program status by 1000 and adding the new error code. For example, in case of 2 errors, at max a six digit number will be returned with the hundred thousand, ten thousand, and hundredths places representing the first error and the other three representing the second error. As max number of errors possible at a time is three, the output will not overflow 4 byte int.