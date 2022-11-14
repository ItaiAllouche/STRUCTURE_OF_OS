#include <commands.h>

using namespace std;



//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(list<job>* jobs, char* lineSize, char* cmdString){
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
	
/*************************************************/

	else if (!strcmp(cmd, "pwd") || !strcmp(cmd, "&pwd") ){
	
        char current_path[MAX_LINE_SIZE];
        getcwd(current_path,sizeof(current_path));
        cout << current_path << endl;
        return SUCCESS	
	}
	
/*************************************************/

	else if (!strcmp(cmd, "mkdir") || !strcmp(cmd, "&mkdir") ){
	
 		
	}

/*************************************************/	

	else if (!strcmp(cmd, "jobs") || !strcmp(cmd, "&jobs") ){
		if(jobs == null)
			return SUCCESS;
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

/*************************************************/

	else if (!strcmp(cmd, "showpid") || !strcmp(cmd, "&showpid") ){
		
		
	}

/*************************************************/

	// fix :
	//	1.what to do with fg procces
	//  2.error invaild args
	else if (!strcmp(cmd, "fg") || !strcmp(cmd, "&fg") ) {
		if(jobs == null){
			cout << "smash error: fg: jobs list is empty" << endl;
			return FAILURE	
		}

		if(num_arg >1 ){
			cout << "smash error: fg: invaild arguments" << endl;
			return FAILURE
		}

		list<job>::iterator list_it = jobs->begin();

		//given job has no id
		if(num_arg == 0){
			list_it= jobs->pop_back();
			cout << list_it->command ":" list_it->job_id << endl;
			list_it->state = FORGROUND_STATE;
			kill(list_it->job_id,SIGCONT);
			waitpid(list_it->job_id);
			return SUCCESS;
		}

		//given job has id
		given_id = atoi(arg[1]);
		while(list_it != jobs->end()){
			if(list_it->job_id = given_id){
				cout << list_it->command ":" list_it->job_id << endl;
				jobs->erase(list_it);
				list_it->state = FORGROUND_STATE;
				kill(list_it->job_id,SIGCONT);
				waitpid(list_it->job_id);
				return SUCCESS;
			}
			list_it++;
		}
		//no matched id in jobs list
		cout << "smash error: fg: job-id "given_id" does not exist" << endl;
		return FAILURE;
	}

/*************************************************/	
	
	// fix :
	//	1.what to do with fg procces
	//  2.error invaild args
	else if (!strcmp(cmd, "bg") || !strcmp(cmd, "&bg") ){
		if(jobs == null){
			cout << "smash error: bg: job-id "arg[1]" does now exist" << endl;
			return FAILURE	
		}

		if(num_arg > 1){
			"smash error: bg: invalid arguments"<< endl;
			return FAILURE
		}

		list<job>::iterator list_it = jobs->begin();
		list<job>::iterator max_id_stopped_job = null;

		//given job has no id
		if(num_arg == 0){
			while(list_it != jobs->end()){
				if(list_it->state == STOP_STATE)
					max_id_stopped_job = list_it;
				list_it++;
			}
			
			//no job is in stop state 
			if(max_id_stopped_job == null){
				cout << "smash error: bg: there are no stopped jobs to resume"
				return FAILURE;	
			}

			cout << max_id_stopped_job->command ":" max_id_stopped_job->job_id << endl;
			max_id_stopped_job->state = BACKGROUND_STATE;
			kill(max_id_stopped_job->job_id,SIGCONT); 
			return SUCCESS;
		}

		//given job has id
		given_id = atoi(arg[1]);
		while(list_it != jobs->end()){
				if(list_it->job_id == given_id){
					if(list_it->state == STOP_STATE){
						cout << list_it->command ":" list_it->job_id << endl;
						list_it->state = BACKGROUND_STATE;
						kill(max_id_stopped_job->job_id,SIGCONT); 
						return SUCCESS;
					}

					//given job is in BACKGROUND STATE
					else{
						cout << "smash error: bg: job-id "list_it->job_id" is already running in the background" << endl;
						return FAILURE

					}
				}
				it++;
			}

			//didnt find the given job
			cout << "smash error: bg: job-id "given_id" does not exist" << endl;
			return FAILURE;
	}
			
/*************************************************/

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


