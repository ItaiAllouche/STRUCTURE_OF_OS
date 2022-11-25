#include "jobs.h"

job::job(): job_id(0), command(""), process_id(0), inserted_to_list_time(time(NULL)), state(0){}

job::job(int job_id, string command, int process_id, time_t inserted_to_list_time, int state):
    job_id(job_id),
    command(command),
    process_id(process_id),
    inserted_to_list_time(inserted_to_list_time),
    state(state){}


job::job(const job& copy_job){
    job_id = copy_job.job_id;
    command = copy_job.command;
    process_id = copy_job.process_id;
    inserted_to_list_time = copy_job.inserted_to_list_time;
    state = copy_job.state;
}

