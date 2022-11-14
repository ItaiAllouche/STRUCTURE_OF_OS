#include <commands.h>

using namespace std;



//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(void* jobs, char* lineSize, char* cmdString){
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++){
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") || !strcmp(cmd, "&cd") ){ 
	
		
	} 
	

	else if (!strcmp(cmd, "pwd") || !strcmp(cmd, "&pwd") ){
	
        char current_path[MAX_LINE_SIZE];
        getcwd(current_path,sizeof(current_path));
        cout << current_path << endl;
        return SUCCESS	
	}
	

	else if (!strcmp(cmd, "mkdir") || !strcmp(cmd, "&mkdir") ){
	
 		
	}

	
	else if (!strcmp(cmd, "jobs") || !strcmp(cmd, "&jobs") ){
        /*note: job has the members:
            job_id
            command
            proccess_id
            seconed_elapsed
            state
        update_listed_time() calcute the time since the job was listed
        */ 
        //iterator to the start of the list
        list<job>::iterator list_it = jobs->begin();
        while(list_it != jobs->end()){
            job->seconed_elapsed = update_listed_time(job);
            printf("[%d] %c : %d %d ", job->job_id, job->command, job->proccess_id, job->seconed_elapsed);
            if(job->state == STOP_STATE)
                printf("(stopped)");
            printf("/n");
            list_it++;
            return SUCCESS	
        }	
	}

	else if (!strcmp(cmd, "showpid") || !strcmp(cmd, "&showpid") ){
	
		
	}

	else if (!strcmp(cmd, "fg") || !strcmp(cmd, "&fg") ) {
	
		
	} 
	
	else if (!strcmp(cmd, "bg") || !strcmp(cmd, "&bg") ){
	
  		
	}
	
	else if (!strcmp(cmd, "quit") || !strcmp(cmd, "&quit") ){
	
   		
	} 

	else{ // external command
	
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1: 
					// Add your code here (error)
					
					/* 
					your code
					*/
        	case 0 :
                	// Child Process
               		setpgrp();
					
			        // Add your code here (execute an external command)
					
					/* 
					your code
					*/
			
			default:
                	// Add your code here
					
					/* 
					your code
					*/
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, void* jobs)
{

	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		// Add your code here (execute a in the background)
					
		/* 
		your code
		*/
		
	}
	return -1;
}


