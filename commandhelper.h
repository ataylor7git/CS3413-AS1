#ifndef COMMANDAHELPER_H
#define COMMANDAHELPER_H

int tokeniseString(char* string, char** tokenArray, char* delimeter);

void waitForPid(int childPid, int* returnCode);

void runCommand(char** tokenArray, int tokenCount, int inPipe, int outPipe, int* currentChildPid, int* currentReturnCode);

#endif