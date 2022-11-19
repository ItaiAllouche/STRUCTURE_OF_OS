/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <list>
#include "commands.h"
#include "signals.h"
#include "jobs.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

list<job> jobs;
int Fg_Proccss_Pid = 0;
string L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE];

 
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[]){
    char cmdString[MAX_LINE_SIZE];
	void(*p_ctrl_c_handler)(int);
    p_ctrl_c_handler = &ctrl_c_hanlder;
	void(*p_ctrl_z_handler)(int);
    p_ctrl_z_handler = &ctrl_z_hanlder;
	
	

	//initilize signal handlers
	if(signal(SIGINT, p_ctrl_c_handler) == SIG_ERR)
		perror("smash error: signal failed");
	if(signal(SIGTSTP, p_ctrl_z_handler) == SIG_ERR)
		perror("smash error: signal failed");

	
	while (1){
		printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);// stores input from stdin in lineSize
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';

		// if bg cmd && exeternal cmd
		if(BgCmd(lineSize) == 0) continue; 
					
		else 
			// run in fg
			ExeCmd(lineSize, false);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
return 0;
}


