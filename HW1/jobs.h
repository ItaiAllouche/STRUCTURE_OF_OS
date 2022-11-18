#ifndef _JOBS_H
#define _JOBS_H

#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <list>

using namespace std;

class job{
    public:
        int job_id;
        string command;
        int process_id;
        clock_t inserted_to_list_time;
        int state;
        bool bg_flag;
        job();
        job(int job_id, string command, int process_id, time_t inserted_to_list_time, int state);
        job(const job& copy_job);
};

#endif