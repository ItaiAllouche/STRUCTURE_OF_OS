#ifndef _COMMANDS_H
#define _COMMANDS_H



#include <unistd.h> 
#include <stdio.h>
#include <cstdio>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iterator>
#include <list>
#include <string>
#include "signals.h"
#include "jobs.h"
#include <iostream>
#include <fstream> 
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define EMPTY 0
#define SUCCESS 0
#define FAILURE 1
#define FORGROUND_STATE 1
#define BACKGROUND_STATE 2
#define STOP_STATE 3
#define MIN_SIG_ID 0
#define MAX_SIG_ID 0

using namespace std;

extern int Fg_Proccss_Pid;
extern string L_Fg_Cmd;
extern list<job> jobs;

int BgCmd(char* lineSize);
int ExeCmd(char* lineSize, bool in_bg);
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool in_bg, char full_command[]);
bool is_built_in_cmd(char* command);

#endif

