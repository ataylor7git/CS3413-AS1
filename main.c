#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <constants.h>
#include <commandhelper.h>
#include <signal.h>
#include <stdbool.h>

bool runExec = true;

void signalHandler(int sigNum)
{
    if(sigNum == SIGTSTP && runExec)
    {
        printf("\nJob Suspended. Type 'fg' or 'bg' to resume.\n");
        runExec = false;
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
                runExec = true;
            else if(!strcmp(argArray[0], "bg"))
                runExec = true;
            //Handle commands
            else if(strcmp(argArray[0], "exit"))
            {
                if(runExec)
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
                    runCommand(argArray, argCount, inPipe, outPipe);
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