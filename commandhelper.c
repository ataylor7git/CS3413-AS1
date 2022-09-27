#include <commandhelper.h>
#include <constants.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

//Tokenises the command into a space delimited array of strings
int tokeniseString(char* string, char** tokenArray, char* delimeter)
{
    char* savePointer;
    int count = 0;

    char* token = strtok_r(string, delimeter, &savePointer);
    while(token != NULL)
    {
        tokenArray[count++] = token;
        token = strtok_r(NULL, delimeter, &savePointer);
    }
    tokenArray[count] = NULL;

    return count;
}

// Runs a command in the token array. Sends the first element as the
// command and the complete token array as the arguments.
// Parent process waits for child to finish
void runCommand(char** tokenArray, int tokenCount, int inPipe, int outPipe)
{
    int returnCode = 0;
    pid_t childPid;
    if((childPid = fork()) == 0)
    {   
        if(dup2(inPipe, STDIN_FILENO) < 0)
            fprintf(stderr,"\nCould not dup2 the input pipe\n");
        if(dup2(outPipe, STDOUT_FILENO) < 0)
            fprintf(stderr,"\nCould not dup2 the output pipe\n");

        returnCode = execvp(tokenArray[0], tokenArray);
    }
    else
    {
        waitpid(childPid, &returnCode, 0 | WUNTRACED);
        //Close the pipes when they are not stdin or stdout
        if(inPipe != STDIN_FILENO)
            close(inPipe);
        if(outPipe != STDOUT_FILENO)
            close(outPipe);
    }
}