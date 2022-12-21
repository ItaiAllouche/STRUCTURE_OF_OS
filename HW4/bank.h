#ifndef _BANK_H
#define _BANK_H

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <map>
#include <list>
#include <string>
#include <cmath>
#include "account.h"
#include "atm.h"

#define MAX_FEE 5
#define FEE_TIME 3
#define PERCENTAGE 100
//#define ACCOUNTS_PRINT_TIME 500000
#define ACCOUNTS_PRINT_TIME 1

using namespace std;
 
class bank{
    public:
        int balance;
        list<atm*>* list_of_atms;
        map<int,account*> map_of_accounts;
        map<int,account*> map_of_deleted_accounts;
        ofstream log_txt_ptr;
        pthread_mutex_t log_print_lock;
        pthread_mutex_t create_lock;
        pthread_mutex_t transfer_lock;
        list<string>* list_of_files;

        //constractor
        bank();

        //distractor
        ~bank();

        //collect fees from every account, fee percentage is rand(MAX_FEE) - every FEE_TIME
        void fee_collection();

        //print the status of all accounts everry ACCOUNTS_PRINT_TIME microsec (0.5 sec)
        void accounts_status_print();

        void execute_actions();

        void free_accounts();

        void free_atms();
};
#endif
