#ifndef COMMANDAHELPER_H
#define COMMANDAHELPER_H

int tokeniseString(char* string, char** tokenArray, char* delimeter);

void runCommand(char** tokenArray, int tokenCount, int inPipe, int outPipe);

#endif