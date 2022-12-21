#ifndef _ATM_H
#define _ATM_H

#include "account.h"
#include <map>
#include <string>

//#define ATM_SLEEP_TIME 100000
#define ATM_SLEEP_TIME 1
#define ACTION_SLEEP_TIME 1

using namespace std;

typedef unsigned int uint;

class atm{
    public:
        uint id;
        string file_path;
        map <int,account*>* map_of_accounts;
        map <int,account*>* map_of_deleted_accounts;
        ofstream* log_txt_ptr;

        //lock for print correctley in log file
        pthread_mutex_t* log_print_lock;

        //lock for create an account safley
        pthread_mutex_t* create_lock;

        //lock for transfer between accounts safley
        pthread_mutex_t* transfer_lock;

    //constarctor
    atm(uint id, string file_path, map<int,account*>* map_of_accounts, map<int,account*>* map_of_deleted_accounts, pthread_mutex_t* log_print_lock, 
        pthread_mutex_t* create_lock, pthread_mutex_t* transfer_lock, ofstream* log_txt_ptr);

    //creatrs a new accounts if isnt exsists - rubin
    //insert it to map if isnt exsist
    void create_account(int acount_id, string password, int balance);

    //deposit "amount" to given account (if exists) -- bruce
    void deposit_to_account(int account_id, string password, int amount);

    //withdrawl "amount" from account (if exists), if amount > balance --> failed -- bruce
    void withdrawl_from_account(int account_id, string password, int amount);

    // check & prints the current balance of account -- bruce
    void balance_check(int account_id, string password);

    //closing account (if exists) -- rubin
    void closing_account(int account_id, string password);

    //transfter from src_account to dest_account if possible (src_amount.balance >= "amount") -- bruce
    void transfer_between_accounts(int src_account_id, string src_password, int dest_account_id, int amount);
 
    //parser the *input* file and call the required methods -- rubin
    void parser_file();

};
#endif

