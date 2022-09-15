#ifndef COMMANDAHELPER_H
#define COMMANDAHELPER_H

int tokeniseCommand(char* command, char** tokenArray);

void runCommand(char** tokenArray, int tokenCount);

#endif