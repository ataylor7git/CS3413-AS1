#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <constants.h>
#include <commandhelper.h>

int main() {
    //Obtain the current directory
    char cwd[FILENAME_MAX];

    //TODO: Remove magic number
    char userInput[100];
    char* command;
    char* savePointer;
    char* tokenArray[100];
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

        int tokenCount = tokeniseCommand(command, tokenArray);

        //Handle cd command
        if(!strcmp(tokenArray[0], "cd") && tokenCount > 1)
            chdir(tokenArray[1]);
        else if(strcmp(tokenArray[0], "exit"))
        {
            runCommand(tokenArray, tokenCount);
        }
  
    }while(strcmp(command, "exit"));
    return EXIT_SUCCESS;
}