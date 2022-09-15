#include <commandhelper.h>
#include <constants.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

//Tokenises the command into a space delimited array of strings
int tokeniseCommand(char* command, char** tokenArray)
{
    char* savePointer;
    int count = 0;

    char* token = strtok_r(command, DELIM, &savePointer);
    while(token != NULL)
    {
        tokenArray[count++] = token;
        token = strtok_r(NULL, DELIM, &savePointer);
    }
    tokenArray[count] = NULL;

    return count;
}

// Runs a command in the token array. Sends the first element as the
// command and the complete token array as the arguments.
// Parent process waits for child to finish
void runCommand(char** tokenArray, int tokenCount)
{
    int returnCode = 0;
    pid_t childPid;
    if((childPid = fork()) == 0)
    {
        returnCode = execvp(tokenArray[0], tokenArray);
    }
    waitpid(childPid, &returnCode, 0 | WUNTRACED);
}