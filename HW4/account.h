#ifndef _ACCOUNT_H
#define _ACCOUNT_H

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

typedef unsigned int uint;

using namespace std;

class account{
    public:
        int id;
        int balance;
        int password;
        account(int id, int balance, int password);

        ~account();

        // lock for abling readers to read safely
        pthread_mutex_t lock_read;

        // lock for abling writes to write safely
        pthread_mutex_t lock_write;

        uint num_of_readers;

        int get_balance();

        bool vallid_password(int password);

        void deposit(int amount);

        bool withdrawl(int amount);
};
#endif