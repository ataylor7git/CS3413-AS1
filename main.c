#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <constants.h>
#include <commandhelper.h>
#include <signal.h>
#include <stdbool.h>

bool runExec = true;
int childPid = 0;
int savedChildPid = -1;
int returnCode;
int savedReturnCode = 0;

//The signal handler. Prints how to start the process again.
//Saves the current child Pid and return code address to be used
//when restarting the process. Restarting the process only works
//without piping (Only works with single commands i.e. ./count)
void signalHandler(int sigNum)
{
    if(sigNum == SIGTSTP && runExec)
    {
        printf("\nJob Suspended. Type 'fg' or 'bg' to resume.\n");
        runExec = false;
        savedChildPid = childPid;
        savedReturnCode = returnCode;
    }
    signal(SIGTSTP, signalHandler);
}

int main() {
    signal(SIGTSTP, signalHandler);
    //Obtain the current directory
    char cwd[FILENAME_MAX];

    //TODO: Remove magic number
    char userInput[100];
    char* command;
    char* savePointer;
    char* tokenArray[COMLIMIT];
    do
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            printf("Trouble getting the current directory");
            return EXIT_FAILURE;
        }

        //Print out the directory
        printf("%s%% ",cwd);

        //Get the user input
        fgets(userInput, 100, stdin);

        //Gets the command without the final new line
        command = strtok_r(userInput, NEWLINE, &savePointer);

        //Separates the commands when piping
        int commandCount = tokeniseString(command, tokenArray, COMDELIM);

        //Creates the pipes (Pipes get overwritten when using SIGTSTP)
        int i;
        int pipes[commandCount * 2];
        for(i = 0; i < commandCount * 2; i += 2)
        {
            int fd[2];
            pipe(fd);
            pipes[i] = fd[0];
            pipes[i + 1] = fd[1];
        }
        
        for(i = 0; i < commandCount; i++)
        {
            char* argArray[ARGLIMIT];
            //Separates the arguments for each command
            int argCount = tokeniseString(tokenArray[i], argArray, ARGDELIM);
            //Handle cd command
            if(!strcmp(argArray[0], "cd") && argCount > 1)
                chdir(argArray[1]);
            else if(!strcmp(argArray[0], "fg"))
            {
                if(savedChildPid != -1)
                {

                    runExec = true;
                    kill(savedChildPid, SIGCONT);
                    waitForPid(savedChildPid, &savedReturnCode);
                }
            }
            else if(!strcmp(argArray[0], "bg"))
            {
                if(savedChildPid != -1)
                {
                    runExec = true;
                    kill(savedChildPid, SIGCONT);
                }
            }
            //Handle commands
            else if(strcmp(argArray[0], "exit"))
            {
                //Only runs a command when SIGTSTP has not caused runExec to false
                //And if the savedChildPid is finished or if there is no savedChildPid
                if(runExec && (kill(savedChildPid, 0) < 0 || savedChildPid == -1))
                {
                    int inPipe;
                    int outPipe;
                    //Set the expected input. On the first command, the input will be stdin
                    if(i == 0)
                        inPipe = STDIN_FILENO;
                    else
                        inPipe = pipes[((i - 1) * 2)];
                    
                    //Set the expected output. On the last command, the output will be stdout
                    if(i == commandCount - 1)
                        outPipe = STDOUT_FILENO;
                    else
                        outPipe = pipes[(i * 2) + 1];
                    runCommand(argArray, argCount, inPipe, outPipe, &childPid, &returnCode);
                    
                }
                else if(i == 0)
                {
                    printf("Not allowed to start new command while you have a job active.\n");
                }
            }
        }
        
    }while(strcmp(command, "exit"));
    return EXIT_SUCCESS;
}