// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */

#include "signals.h"
using namespace std;

void ctrl_c_hanlder(int pid){
   cout << "smash: caught ctrl-C " << endl;

   if(pid > 0 && Fg_Proccss_Pid != smash_pid){
      if(kill(Fg_Proccss_Pid, SIGKILL) == SUCCESS)
         cout << "smash: process " << Fg_Proccss_Pid << " was killed" << endl; 
      else
         perror("smash error: kill failed");
      
   }
   return;
}

//cant recive list as arg, need to fix**
void ctrl_z_hanlder(int pid){
   cout << "smash: caught ctrl-Z" << endl;
   if(pid > 0 && Fg_Proccss_Pid != smash_pid){
      if(kill(Fg_Proccss_Pid, SIGSTOP) == SUCCESS){
         cout << "smash: process " << Fg_Proccss_Pid << " was stopped" << endl; 
         list<job>::iterator job_iterator = jobs.end();
         job_iterator--;
         int new_job_id = (job_iterator->job_id) + 1;
         time_t curr_time = time(NULL);
         job new_job(new_job_id, L_Fg_Cmd, Fg_Proccss_Pid, curr_time, STOP_STATE);
         update_list();
         jobs.push_back(new_job);

         //now smash process runs in fg
         Fg_Proccss_Pid = smash_pid;
      }
      else
         perror("smash error: kill failed");  
   }
   return;
}

