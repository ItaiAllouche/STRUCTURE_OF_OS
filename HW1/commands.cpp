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
	if (!strcmp(cmd, "showpid") || !strcmp(cmd, "&showpid") ){
		unsigned int smash_pid = getpid();
		cout << "smash pid is" << smash_pid << endl;
		return SUCCESS;		
	}

/*************************************************/
	else if (!strcmp(cmd, "pwd") || !strcmp(cmd, "&pwd") ){
	
        char current_path[MAX_LINE_SIZE+1];
        getcwd(current_path,sizeof(current_path));
        cout << current_path << endl;
        return SUCCESS	
	}
		
/*************************************************/	
	else if (!strcmp(cmd, "cd") || !strcmp(cmd, "&cd") ){ 
		if(num_arg > 1 || num_arg <1){
			cout << "too many arguments" << endl;
			return FAILURE
		}

		//the user wants change pwd to previus one
		if(!strcmp(arg[1], "-")){
			char previous_pwd[MAX_LINE_SIZE+1];
			
			//check for previous pwd
			if(getcwd(previous_pwd,MAX_LINE_SIZE+1) == null){
				cout << "smah error: OLDPWD not set" << endl;
				return FAILURE;
			}

			else{
				if(chdir(previous_pwd) == 0)
					return SUCCESS;	
				
				return FAILURE; 
			}
		}

		//change path to given path
		if(chdir(arg[1]) == 0)
			return SUCCESS;

		return FAILURE;	
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
		jobs (list) is sorted by job_id var;
        */ 
        //iterator to the start of the list
        list<job>::iterator list_it = jobs->begin();
        while(list_it != jobs->end()){
            list_it->update_listed_time();
			cout << "["list_it->job_id"] " << list_it->command " : " << list_it->proccess_id << " "<< list_it->seconed_elapsed;  
            if(job->state == STOP_STATE)
                cout << " (stopped)";
            cout << endl;
            list_it = list_it->next();
		}
            return SUCCESS		
	}

/*************************************************/
	//check what if kill failed
	//chack which signals do we have
	else if(!strcmp(cmd, "kill") || !strcmp(cmd, "&kill") ){
		if(num_arg != 2){
			cout << "smash error: kill: invalid arguments" << endl;
			return FAILURE;	
		}

		else{
			int signum = atoi(arg[1]);
			int job_id = atoi(arg[2]);
			
			if(signum < MIN_SIG_ID || !arg[1] || job_id < 0 || !arg[2] ){ 
				cout << "smash error: kill: invalid arguments" << endl;
				return FAILURE;
			}
			
			list<job>::iterator job_iterator = jobs->begin();
			while( job_iterator != jobs->end()){
				//given job was found
				if(job_iterator->job_id == job_id){
						kill(job_iterator->Process_id,signum)	
						 if(signum == SIGTSTP || signum == SIGSTOP || signum == SIGIOT)
							job_iterator->state = STOP_STATE;

						else if(signum == SIGKILL || signum == SIGBUS || signum == SIGHUP || signum == SIGTERM || signum == SIGINT)
							jobs->erase(job_iterator);
						
						else if(signum == SIGCONT){
							job_iterator->state = BACKGROUND_STATE;
						}

						cout << "signal number " << signum << "was sent to pid " << job_iterator->process_id << endl; 
						return SUCCESS;
					}
				job_iterator = job_iterator->next();
			}
			cout << "smash error: kill: job-id " << job_iterator->job_id << " does not exist" << endl;
			return FAILURE;
		}
	}
	/*************************************************/

	//	1.what to do with fg procces
	//  2.error invaild args
	else if (!strcmp(cmd, "fg") || !strcmp(cmd, "&fg") ) {
		if(jobs == null && num_arg == 0){
			cout << "smash error: fg: jobs list is empty" << endl;
			illegal_cmd = false;
			return FAILURE	
		}

		if(num_arg >1 ){
			cout << "smash error: fg: invaild arguments" << endl;
			illegal_cmd = false;
			return FAILURE
		}

		list<job>::iterator list_it = jobs->begin();

		//given job has no id
		if(num_arg == 0){
			// max job->id in list
			list_it= jobs->pop_back();
			cout << list_it->command " : " list_it->job_id << endl;
			list_it->state = FORGROUND_STATE;
			kill(list_it->job_id, SIGCONT);
			waitpid(list_it->job_id ,null, WUNTRANCED);
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
				waitpid(list_it->job_id ,null, WUNTRANCED);
				return SUCCESS;
			}

			list_it = list_it->next();
		}
		//no matched id in jobs list
		cout << "smash error: fg: job-id "<< given_id <<" does not exist" << endl;
		return FAILURE;
	}

/*************************************************/	
	
	// fix :
	//	1.what to do with fg procces
	//  2.error invaild args
	else if (!strcmp(cmd, "bg") || !strcmp(cmd, "&bg") ){
		if(jobs == null && num_arg == 1){
			cout << "smash error: bg: job-id "arg[1]" does now exist" << endl;
			illegal_cmd = false;
			return FAILURE	
		}

		if(num_arg > 1){
			"smash error: bg: invalid arguments"<< endl;
			illegal_cmd = false;
			return FAILURE
		}

		list<job>::iterator list_it = jobs->begin();
		list<job>::iterator max_id_stopped_job = null;

		//given job has no id
		if(num_arg == 0){
			while(list_it != jobs->end() && list_it != null){
				if(list_it->state == STOP_STATE)
					if(max_id_stopped_job ==null || max_id_stopped_job->job_id <= list_it->job_id)
						max_id_stopped_job = list_it; 
				list_it = list_it->next();
			}
			
			//no job is in stop state 
			if(max_id_stopped_job == null){
				cout << "smash error: bg: there are no stopped jobs to resume"
				return FAILURE;	
			}

			cout << max_id_stopped_job->command " : " max_id_stopped_job->job_id << endl;
			max_id_stopped_job->state = BACKGROUND_STATE;
			kill(max_id_stopped_job->job_id,SIGCONT); 
			return SUCCESS;
		}

		//given job has id
		given_id = atoi(arg[1]);
		while(list_it != jobs->end()){
				if(list_it->job_id == given_id){
					if(list_it->state == STOP_STATE){
						cout << list_it->command " : " list_it->job_id << endl;
						list_it->state = BACKGROUND_STATE;
						kill(max_id_stopped_job->job_id,SIGCONT); 
						return SUCCESS;
					}

					//given job is in BACKGROUND STATE
					else{
						cout << "smash error: bg: job-id "<< list_it->job_id << " is already running in the background" << endl;
						return FAILURE
					}
				}
				list_it = list_it->next();
			}

			//didnt find the given job
			cout << "smash error: bg: job-id " << given_id << " does not exist" << endl;
			return FAILURE;
	}
			
/*************************************************/

	else if (!strcmp(cmd, "quit") || !strcmp(cmd, "&quit")){
		// no relevant arguments were passed
		if(num_arg == 0 || strcmp("kill", args[1]) != 0)
			exit(0);

		// "kill" argument was passed		
		list<job>::iterator list_it = jobs->begin();
		while(list_it != jobs->end()){
			clock_t start;
			int status;
			bool terminated = false;
			kill(list_it->job_id, SIGTERM);
			//normalized to sec
			start = clock()/CLOCKS_PER_SEC;
			waitpid(list_it->job_id, status, WNOHANG)
			
			while(clock()/CLOCKS_PER_SEC <= start+5){
				// child procces was terminated
				if(WIFSIGNALED(status)){
					terminated = true;	
					break;
				}
			}

			//process wasnt terminated -> sending SIGKILL
			if((clock()/CLOCKS_PER_SEC > start+5) && !terminated)
				kill(list_it->job_id, SIGKILL);

			list<job>::iterator job_to_erase = list_it;
			list_it++;
			jobs->erase(job_to_erase);
			if(jobs->size() == EMPTY)
				break;	
		}
		exit(0)		
	} 

/*************************************************/

	else if (!strcmp(cmd, "diff") || !strcmp(cmd, "&diff")){
		if(num_arg != 2){
			cout << "smash error: diff: invalid arguments" << endl;
			illegal_cmd = false;
			return FAILURE;
		}

		ifstream f1;
		ifstream f2;
		f1.open(arg[1], ifstream:ate | ifstream:binary);
		f2.open(arg[2], ifstream:ate | ifstream:binary);

		// checking both files
		if(!f1 || !f2){
			cout << "1" << endl;
			f1.close();
			f2.close();
			return SUCCESS;
		}

		//files have diffrent size
		if(f1.tellg() != f2.tellg()){
			cout << "1" << endl;
			f1.close();
			f2.close();
			return SUCCESS;
		}

		ifstream f1_start;
		f2.close();
		f1_start.open(arg[1], ifstream:in | ifstream:binary);
		f2.open(arg[2], ifstream:in | ifstream:binary);
		//initilize iterators to start of f1&f2 and another one to the end of f1
		istream_iterator<char> f1_start_iterator (f1_start);
		istream_iterator<char> f2_iterator (f2);
		istream_iterator<char> f1_iterator (f1);
		
		if(equal(f1_start_iterator, f1_iterator, f2_iterator))
			cout << "0" << endl;
		else
			cout << "1" << endl;
		
		f1.close();
		f2.close();
		f1_start.close();
		return SUCCESS;	
	}

/*************************************************/

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


