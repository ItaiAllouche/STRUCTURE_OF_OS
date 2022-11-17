// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */

#include "signals.h"
using namespace std;

void ctrl_c_hanlder(int pid){
   cout << "smash: caught ctrl-C" << endl;

   if(pid > 0){
      if(kill(Fg_Proccss_Pid, SIGKILL) == SUCCESS)
         cout << "smash: process " << Fg_Proccss_Pid << " was killed"; 
      else
         perror("smash error: kill failed");
      
   }
   return;
}

//cant recive list as arg, need to fix**
void ctrl_z_hanlder(int pid){
   cout << "smash: caught ctrl-Z" << endl;

   if(pid > 0){
      if(kill(Fg_Proccss_Pid, SIGSTOP) == SUCCESS){
         cout << "smash: process " << Fg_Proccss_Pid << " was stopped"; 
         char cmd [MAX_LINE_SIZE] = "ctrlZ"
         list<job>::iterator job_iterator = jobs->end();
         int new_job_id= job_iterator->job_id + 1;
         time_t curr_time = time();
         job new_job(new_job_id, L_Fg_Cmd, Fg_Proccss_Pid, curr_time, STOP_STATE);
         jobs->push_back(new_job);
      }
      else
         perror("smash error: kill failed");  
   }
   return;
}

