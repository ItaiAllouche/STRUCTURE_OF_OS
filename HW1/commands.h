#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <cstream>
#include <iostream>
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include <commands.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define EMPTY 0
#define SUCCESS 0
#define FAILURE 1
#define FORGROUND_STATE 1
#define BACKGROUND_STATE 2
#define STOP_STATE 3

typedef enum { FALSE , TRUE } bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, void* jobs);
int ExeCmd(void* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
#endif

