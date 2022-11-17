#ifndef _SIGS_H
#define _SIGS_H

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <list>
#include <string>
#include <iostream>
#include "commands.h"
#include "jobs.h"

#define MAX_LINE_SIZE 80
#define SUCCESS 0
#define STOP_STATE 3

extern int Fg_Proccss_Pid;
extern string L_Fg_Cmd;
extern list<job> jobs;

void ctrl_z_hanlder(int pid);
void ctrl_c_hanlder(int pid);
#endif

