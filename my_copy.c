// @author: Prajas Naik and Kalash Shah
// Basic command line program to implement file copy using system calls
//Basic Header Files

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Error Definitions

#define     E_OK                        0
#define     E_INCORRECT_ARG_NO          201
#define     WRITE_OPERATION_INCOMPLETE  202
#define     E_INCORRECT_ARGS            203
#define     E_GENERAL_ERROR             -1

//Defined for ease of reading

#define     STDIN                       0
#define     STDOUT                      1

// Define Limits
#define     BYTES_TO_READ               1024
#define     MAX_ARG_NO                  5
#define     ERROR_CODE_OFFSET           1000

// Global Variables
int     ec                      = E_OK;
char    *inputDestination;
char    *outputDestination;

// Function Declarations
int     Help                    (char *);
int     ProcessCommandLine      (char **);
int     CopyFile                ();

// Main Function
int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        ec = Help(argv[0]); //To call help when no arguments given
    }
    else if (argc == MAX_ARG_NO)
    {
        ec = ProcessCommandLine(argv); //Extracts input and output destination
        if (ec == E_OK)
        {
            ec = CopyFile(); //Copies from input to output
        }
    }
    else
    {
        ec = E_INCORRECT_ARG_NO;
    }
    return ec;
}

// function: ProcessCommandLine
//      Takes the command line arguments and extracts the input and output destination
//      Order of -i and -o does not matter
// @param: Pointer to character array containing command line arguments
// @returns: int error code
int ProcessCommandLine(char *commandLineArguments[])
{
    int argno = 1;
    while(argno < MAX_ARG_NO)
    {
        switch (commandLineArguments[argno][1])
        {
        case 'i':
            if (commandLineArguments[argno + 1][0] == '-')
            {
                inputDestination = "-";
            }
            else 
            {
                inputDestination = commandLineArguments[argno + 1];
            }
            argno += 2;
            break;
        
        case 'o':
            if (commandLineArguments[argno + 1][0] == '-')
            {
                outputDestination = "-";
            }
            else
            {
                outputDestination = commandLineArguments[argno + 1];
            }
            argno += 2;
            break;
        
        default:
            return E_INCORRECT_ARGS;
            break;
        }
    } 
    return E_OK; 
}

// function: CopyFile
//      Copies from the input to the output destination. Opens Files using open
//      system call and copies by using read/write system calls. Detects appropriate
//      errors and returns unique error codes
// @param: None
// @returns: int error code
int CopyFile ()
{
    // Opening input and output files
    int status = E_OK;
    int inputFileDescriptor;
    int outputFileDescriptor;
    if (strcmp(inputDestination, "-") == 0)
    {
        inputFileDescriptor = STDIN;
    }
    else
    {
        inputFileDescriptor = open(inputDestination, O_RDONLY);
        if (inputFileDescriptor == E_GENERAL_ERROR)
        {
            status = errno;
            return status;
        }
    }
    if (strcmp(outputDestination, "-") == 0)
    {
        outputFileDescriptor = STDOUT;
    }
    else
    {
        outputFileDescriptor = open(outputDestination, O_WRONLY | O_CREAT, S_IRWXU);
        if (inputFileDescriptor == E_GENERAL_ERROR)
        {
            status = errno;
            int error2 = close(inputFileDescriptor);
            if (error2 == E_GENERAL_ERROR)
                return status * ERROR_CODE_OFFSET + errno; // If more than one error occurs, the first error is multiplied by 1000 and the second error is added. Appropriately handled in bash scriptS
            return status;
        }
    }
    // Copying from input to output

    int numBytesWritten = 0;
    char buffer[BYTES_TO_READ];
    int numBytesRead =  numBytesRead = read(inputFileDescriptor, &buffer, BYTES_TO_READ);

    while (numBytesRead != 0)
    {
        if (numBytesRead == E_GENERAL_ERROR)
        {
            status = errno;
            int error2 = close(inputFileDescriptor);
            if (error2 == E_GENERAL_ERROR)
            {
                status = status * ERROR_CODE_OFFSET + errno;
                error2 = close(outputFileDescriptor); // If more than one error occurs, the first error is multiplied by 1000 and the second error is added. Appropriately handled in bash script/
                if(error2 == E_GENERAL_ERROR)
                    return status * ERROR_CODE_OFFSET + errno;
                else return status;   
            }
            error2 = close(outputFileDescriptor); 
            if (error2 == E_GENERAL_ERROR)
                return status * ERROR_CODE_OFFSET + errno;
            return status;
        }
        else 
        {
            numBytesWritten = write(outputFileDescriptor, &buffer, numBytesRead);
            if (numBytesWritten == E_GENERAL_ERROR)
            {
                status = errno;
                int error2 = close(inputFileDescriptor);
                if (error2 == E_GENERAL_ERROR)
                {
                    status = status * ERROR_CODE_OFFSET + errno;
                    error2 = close(outputFileDescriptor); // If more than one error occurs, the first error is multiplied by 1000 and the second error is added. Appropriately handled in bash script/
                    if(error2 == E_GENERAL_ERROR)
                        return status * ERROR_CODE_OFFSET + errno;
                    else return status;   
                }
                error2 = close(outputFileDescriptor);
                if (error2 == E_GENERAL_ERROR)
                    return status * ERROR_CODE_OFFSET + errno;
                return status;
            }
            else if (numBytesWritten != numBytesRead)
            {
              
                status = WRITE_OPERATION_INCOMPLETE;
                int error2 = close(inputFileDescriptor);
                if (error2 == E_GENERAL_ERROR)
                {
                    status = status * ERROR_CODE_OFFSET + errno;
                    error2 = close(outputFileDescriptor);
                    if(error2 == E_GENERAL_ERROR)
                        return status * ERROR_CODE_OFFSET + errno; // If more than one error occurs, the first error is multiplied by 1000 and the second error is added. Appropriately handled in bash script/
                    else return status;   
                }
                error2 = close(outputFileDescriptor);
                if (error2 == E_GENERAL_ERROR)
                    return status * ERROR_CODE_OFFSET + errno;
                return status;
            }
            else 
            {
                numBytesRead = read(inputFileDescriptor, &buffer, BYTES_TO_READ);
            }
        }

    }
    // cleanup
    status = close(inputFileDescriptor);
    if (status == E_GENERAL_ERROR)
    {
        status = errno;
        int error2 = close(outputFileDescriptor);
        if (error2 == E_GENERAL_ERROR)
            return status * ERROR_CODE_OFFSET + errno;
    }
    status = close(outputFileDescriptor);
    if (status == E_GENERAL_ERROR)
    {
        status = errno;
    }
    return status;
}

// function: Help
//      Prints how to use the executable
// @param: pointer to character array containing command line arguments
// @returns: int error code
int Help(char *commandLineArguments)
{
    char *helpMessage = "flame_cp  -i <source file pathname> -o <destination file pathname>\n";
    int bytesWritten = write(STDOUT, helpMessage, strlen(helpMessage));
    if (bytesWritten == E_GENERAL_ERROR)
    {
        return errno;
    }
    else if (bytesWritten < strlen(helpMessage))
    {
        return WRITE_OPERATION_INCOMPLETE;
    }
    else
        return E_OK;
}