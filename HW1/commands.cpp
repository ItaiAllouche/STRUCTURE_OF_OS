#include "commands.h"

using namespace std;

char previous_path[MAX_LINE_SIZE] = "";

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(char* lineSize, bool in_bg){
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
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
	if (!strcmp(cmd, "showpid")){
		unsigned int smash_pid = getpid();
		cout << "smash pid is " << smash_pid << endl;
		return SUCCESS;		
	}

/*************************************************/
	else if (!strcmp(cmd, "pwd")){
	
        char current_path[MAX_LINE_SIZE+1];
        getcwd(current_path,sizeof(current_path));
        cout << current_path << endl;
        return SUCCESS;	
	}
		
/*************************************************/	
	else if (!strcmp(cmd, "cd")){ 
		if(num_arg > 1){
			cout << "smash error: cd: too many arguments" << endl;
			return FAILURE;
		}
		char temp_path[MAX_LINE_SIZE];
		//saving current path
		getcwd(temp_path, sizeof(temp_path));

		//the user wants change pwd to previuos one
		if(!strcmp(args[1], "-")){
			//no previous path 
			if(!strcmp(previous_path,"")){
				cout << "smah error: OLDPWD not set" << endl;
				return FAILURE;
			}
			else{
				if(chdir(previous_path) == 0){
					strcpy(previous_path, temp_path);
					return SUCCESS;
				}
				return FAILURE; 
			}
		}

		//change path to given path
		if(chdir(args[1]) != 0){
			perror("smash error: chdir failed");
			return FAILURE;	
		}
		else{
			strcpy(previous_path, temp_path);
			return SUCCESS;
		}
		
	} 
	
/*************************************************/

	else if (!strcmp(cmd, "jobs")){
		if(jobs.size() == 0){
			return SUCCESS;
		}

		//remove finished jobs if existing.
		update_list();

   
        //iterator to the start of the list
        list<job>::iterator list_it = jobs.begin();
        while(list_it != jobs.end()){
			double time_since_inserted = difftime(time(NULL), list_it->inserted_to_list_time);
			cout << "[" << list_it->job_id << "] " << list_it->command  <<  " : " << list_it->process_id << " " << time_since_inserted;
            if(list_it->state == STOP_STATE)
                cout << " secs (stopped)";
            cout << endl;
            list_it++;
		}
        return SUCCESS;	
	}

/*************************************************/
	//check what if kill failed
	//chack which signals do we have
	else if(!strcmp(cmd, "kill")){
		if(num_arg != 2){
			cout << "smash error: kill: invalid arguments" << endl;
			return FAILURE;	
		}

		else{
			////expected signum to be negative (suffix -) -> now signum should be positive
			int signum = (-1)*atoi(args[1]);
			int job_id = atoi(args[2]);
			
			if(signum < MIN_SIG_ID || job_id < 0){ 
				cout << "smash error: kill: invalid arguments" << endl;
				return FAILURE;
			}
			
			list<job>::iterator job_iterator = jobs.begin();
			while( job_iterator != jobs.end()){
				//given job was found
				if(job_iterator->job_id == job_id){
						if(kill(job_iterator->process_id ,signum) != SUCCESS){
							perror("smash error: kill failed");
							return FAILURE;
						}	 	
						 if(signum == SIGTSTP || signum == SIGSTOP || signum == SIGIOT)
							job_iterator->state = STOP_STATE;

						else if(signum == SIGKILL || signum == SIGBUS || signum == SIGHUP || signum == SIGTERM || signum == SIGINT)
							jobs.erase(job_iterator);
						
						else if(signum == SIGCONT){
							job_iterator->state = BACKGROUND_STATE;	 
						}

						cout << "signal number " << signum << " was sent to pid " << job_iterator->process_id << endl; 
						return SUCCESS;
					}
				job_iterator++;
			}
			cout << "smash error: kill: job-id " << job_iterator->job_id << " does not exist" << endl;
			return FAILURE;
		}
	}
	/*************************************************/

	//	1.what to do with fg procces
	//  2.error invaild args
	else if (!strcmp(cmd, "fg")) {
		if(jobs.size() == 0 ){
			cout << "smash error: fg: jobs list is empty" << endl;
			return FAILURE;	
		}

		if(num_arg >1 ){
			cout << "smash error: fg: invaild arguments" << endl;
			return FAILURE;
		}

		list<job>::iterator list_it;

		//given job has no id
		if(num_arg == 0){
			// max job->id in list
			list_it = jobs.end();
			list_it--;
			jobs.pop_back();
			cout << list_it->command << " : " << list_it->process_id<< endl;
			list_it->state = FORGROUND_STATE;
			if(kill(list_it->process_id, SIGCONT) != SUCCESS){
				perror("smash error: kill failed");
				return FAILURE;
			}
			waitpid(list_it->process_id ,NULL, WUNTRACED);
			L_Fg_Cmd = list_it->command;
			Fg_Proccss_Pid = list_it->process_id;
			return SUCCESS;
		}

		list_it = jobs.begin();
		//given job has id
		int given_id = atoi(args[1]);
		while(list_it != jobs.end()){
			if(list_it->job_id == given_id){
				cout << list_it->command << " : " << list_it->process_id << endl;
				string temp  = list_it->command;
				jobs.erase(list_it);
				list_it->state = FORGROUND_STATE;
				if(kill(list_it->process_id, SIGCONT) != SUCCESS){
					perror("smash error: kill failed");
					return FAILURE;
				}
				L_Fg_Cmd = temp;
				Fg_Proccss_Pid = list_it->process_id;
				waitpid(list_it->process_id ,NULL, WUNTRACED);
				return SUCCESS;
			}

			list_it++;
		}
		//no matched id in jobs list
		cout << "smash error: fg: job-id "<< given_id <<" does not exist" << endl;
		return FAILURE;
	}

/*************************************************/	
	else if (!strcmp(cmd, "bg")){
		//list is e,empty and an arg was passed
		if(jobs.size() == 0 && num_arg == 1){
			cout << "smash error: bg: job-id "<< args[1] <<" does now exist" << endl;
			return FAILURE;
		}
		//too many args
		if(num_arg > 1){
			cout << "smash error: bg: invalid arguments"<< endl;
			return FAILURE;
		}

		//list is empty and no argument was passed.
		if(jobs.size() == 0 && num_arg == 0){
			cout << "smash error: bg: there are no stopped jobs to resume" << endl;
				return FAILURE;
		}

		// list is not empty
		list<job>::iterator list_it = jobs.begin();
		list<job>::iterator max_id_stopped_job;
		bool job_found = false;

			//no arg was passed.
            if(num_arg == 0){
				while(list_it != jobs.end()){
						//have not found a stopped job yet
					if(list_it->state == STOP_STATE && !job_found){
						max_id_stopped_job = list_it;
						job_found = true;
					}
						//stopped job was alreaday found
					else if(list_it->state == STOP_STATE && job_found){
						if((max_id_stopped_job->job_id) < (list_it->job_id)){
							max_id_stopped_job = list_it; 
						}
					list_it++;
					}
				}
				
				//no job is at stop state 
				if(!job_found){
					cout << "smash error: bg: there are no stopped jobs to resume" << endl;
					return FAILURE;	
				}
				if(kill(max_id_stopped_job->process_id, SIGCONT) != SUCCESS){
					perror("smash error: kill failed");
					return FAILURE;
				}
				cout << max_id_stopped_job->command  << " : " << max_id_stopped_job->job_id << endl;
				max_id_stopped_job->state = BACKGROUND_STATE;
				return SUCCESS;
			}

		//arg was passed
		int given_id = atoi(args[1]);
		while(list_it != jobs.end()){
				if(list_it->job_id == given_id){
					if(list_it->state == STOP_STATE){
						if(kill(list_it->process_id, SIGCONT)!= SUCCESS){
							perror("smash error: kill failed");
							return FAILURE;
						} 
						cout << list_it->command << " : " << list_it->job_id << endl;
						list_it->state = BACKGROUND_STATE;
						return SUCCESS;
					}

					//given job is in BACKGROUND STATE
					else{
						cout << "smash error: bg: job-id "<< list_it->job_id << " is already running in the background" << endl;
						return FAILURE;
					}
				}
				list_it++;
			}

			//didnt find the given job
			cout << "smash error: bg: job-id " << given_id << " does not exist" << endl;
			return FAILURE;
	}
			
/*************************************************/

	else if (!strcmp(cmd, "quit")){
		// no relevant arguments were passed
		if(num_arg == 0 || strcmp("kill", args[1]) != 0)
			exit(0);

		// "kill" argument was passed		
		list<job>::iterator list_it = jobs.begin();
		while(list_it != jobs.end()){
			clock_t start;
			int status;
			bool terminated = false;
			if(kill(list_it->process_id, SIGTERM) != SUCCESS){
				perror("smash error: kill failed");
				return FAILURE;
			}
			//normalized to sec
			start = clock()/CLOCKS_PER_SEC;
			waitpid(list_it->process_id, &status, WNOHANG);
			
			while(clock()/CLOCKS_PER_SEC <= start+5){
				// child procces was terminated
				if(WIFSIGNALED(status)){
					terminated = true;	
					break;
				}
			}

			//process wasnt terminated -> sending SIGKILL
			if((clock()/CLOCKS_PER_SEC > (start + 5)) && !terminated){
				if(kill(list_it->process_id, SIGKILL) != SUCCESS){
					perror("smash error: kill failed");
					return FAILURE;
				}

			}

			list<job>::iterator job_to_erase = list_it;
			list_it++;
			jobs.erase(job_to_erase);
			if(jobs.size() == EMPTY){
				exit(0);
			}
		}
		exit(0);	
	} 

/*************************************************/

	else if (!strcmp(cmd, "diff")){
		if(num_arg != 2){
			cout << "smash error: diff: invalid arguments" << endl;
			return FAILURE;
		}

		ifstream f1(args[1], ifstream::ate | ifstream::binary);
		ifstream f2(args[2], ifstream::ate | ifstream::binary);

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
		f1_start.open(args[1], ifstream::in | ifstream::binary);
		f2.open(args[2], ifstream::in | ifstream::binary);
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
	// external command
	else {
 		ExeExternal(args, cmd, in_bg, lineSize);
	 	//return 0;
	}
	/*if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}*/
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool in_bg, char full_command[]){
	int status;
	int pID;
	switch(pID = fork()){
	case -1: 
			perror("smash error: fork failed");
			break; 
	case 0 :
			setpgrp();
			if(execv(args[0],args) == -1){
				perror("smash error: execv failed");
				if(in_bg){
					printf("smash > ");
				}
				exit(1);
			}
			else{
				exit(0);
			}
			break;

	
	default:
			//process is in background
			if(in_bg){
				if(jobs.size() < 100){	
					list<job>::iterator job_iterator = jobs.end();
					job_iterator--;
					int new_job_id = (job_iterator->job_id) + 1;
					time_t  curr_time = time(NULL);
					string command =  full_command;
					job new_job = job(new_job_id, command, pID, curr_time, BACKGROUND_STATE);
					update_list();
					jobs.push_back(new_job);
					}

				}
			
			else{
				L_Fg_Cmd = full_command;
				Fg_Proccss_Pid = pID;
				waitpid(pID, NULL, WUNTRACED);
			}
			break;            	
	}	
	
}

//**************************************************************************************
// function name: is_built_in_cmd
// Description: checks if given cmd is a built in cmd
// Parameters: char* command
// Returns: true if given cmd is bulit in, and false if not.
//**************************************************************************************
bool is_built_in_cmd(char* command){
	if( !strcmp(command, "showpid") || !strcmp(command, "pwd") || !strcmp(command, "cd") || !strcmp(command, "jobs") || !strcmp(command, "kill") || 
		!strcmp(command, "fg") || !strcmp(command, "bg") || !strcmp(command, "quit") || !strcmp(command, "diff")){
			return true;
		}
		return false;
}


//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize){
	char* command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	char full_command[MAX_LINE_SIZE+1];
	strcpy(full_command,lineSize);
	full_command[strlen(lineSize)-1]='\0';
	if(lineSize[strlen(lineSize)-2] == '&'){
		lineSize[strlen(lineSize)-2] = '\0';
		char temp_lineSize[strlen(lineSize)];
		strcpy(temp_lineSize,lineSize);
		command = strtok(temp_lineSize, delimiters);
		if(command == NULL)
			return SUCCESS;

		args[0] = command;

		for(int i=1; i<MAX_ARG; i++)
			args[i] = strtok(NULL, delimiters);

		//not built in cmd && bg cmd
		if(!is_built_in_cmd(command)){
			ExeExternal(args, command, true, full_command);
			return 0;
		}
		else{
			return -1;
		}	
	}
	return -1;
}

//**************************************************************************************
void update_list(){
	if(jobs.size() == EMPTY){
		return;
	}

	list<job>::iterator list_it = jobs.begin();

	while(list_it != jobs.end()){
		if(waitpid(list_it->process_id, NULL, WNOHANG) != 0){
			list_it = jobs.erase(list_it);
		}
		else{
			list_it++;
		}
	}
	return;
}
//**************************************************************************************
