#ifndef _ATM_H
#define _ATM_H

#include "account.h"

#define 100 TIME_TO_WAKE;


using namespace std;

typedef unsigned int uint;

class atm{
    private:
        uint id;
        unordered_map<int,account>* map_of_accounts;
        string file_path;
        FILE* log_txt_ptr;

    public:
    //lock for print correctley in log file
    pthread_mutex_t* log_print_lock;

    // lock for checking exsiting of account (cucorencey->critical section)
    pthread_mutex_t* accounts_lock

    //constarctor -- rubin
    atm(uint id, string file_path, unorderd_map<int,account>* map_of_accounts, pthread_mutex_t* log_print_lock, pthread_mutex_t* accounts_lock, );

    //distractor 
    ~atm();


    //class methods
    // ********************************************attaintion for mutexs - before every write to log file and map,use log_txt_ptr mebmber*******************************************

    //creatrs a new accounts if account isnt exists - rubin
    account create_account(int acount_id, int balance, int password);

    //deposit "amount" to given account (if exists) -- bruce
    bool deposit_to_account(int account_id, int password, int amount);

    //withdrawl "amount" from account (if exists), if amount > balance --> failed -- bruce
    bool withdrawl_from_account(int account_id, int password, int amount);

    // check & prints the current balance of account -- bruce
    void balance_check(int account_id, int password);

    //closing account (if exists) -- rubin
    bool closing_account(int account_id, int password);

    //transfter from src_account to dest_account if possible (src_amount.balance >= "amount") -- bruce
    void transfer_between_accounts(int src_account_id, int src_password, int dest_account_id, int dest_password, int amount);

    //check if acount alreday exists in the  hash map -- bruce
    bool account_already_exists(int acount_id);
    
    //parser the *input* file and call the required methods -- rubin
    void parser_file();

}
#endif