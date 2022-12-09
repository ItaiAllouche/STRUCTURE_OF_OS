#include "atm.h"

atm::atm(uint id, string file_path, map <int,account*>* map_of_accounts, pthread_mutex_t* log_print_lock, pthread_mutex_t* accounts_lock, ofstream* log_txt_ptr){
    this->id = id;
    this->map_of_accounts = map_of_accounts;
    this->file_path = file_path;
    pthread_mutex_init(&(*log_print_lock), NULL);
    pthread_mutex_init(&(*accounts_lock), NULL);
    this->log_txt_ptr = log_txt_ptr;
}

bool atm::account_already_exists(int account_id){
    pthread_mutex_lock(accounts_lock);
    sleep(ACTION_SLEEP_TIME);

    if(map_of_accounts->find(account_id) == map_of_accounts->end()){
        pthread_mutex_unlock(accounts_lock);
        return false;
    }
    pthread_mutex_unlock(accounts_lock);
    return true;
}

bool atm::create_account(int account_id, int balance, int password){
    if(account_already_exists(account_id)){
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << account_id << ": Your transaction failed - account with the same id exists" << endl;
        pthread_mutex_unlock(log_print_lock);
        return false;
    }
    
    else{
        pthread_mutex_lock(accounts_lock);
        account* new_account = new account(account_id, balance, password);
        map_of_accounts->insert(pair<int, account*>(account_id, new_account));
        pthread_mutex_unlock(accounts_lock);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << this->id << ": New account id is " <<  account_id << " with password " << password << " and initial balance " << balance << endl;
        pthread_mutex_unlock(log_print_lock);
        return true;
    }
}

bool atm::deposit_to_account(int account_id, int password, int amount){
    map <int, account*>::iterator it;
    pthread_mutex_lock(accounts_lock);
    sleep(ACTION_SLEEP_TIME);
    it = map_of_accounts->begin();
    while(it != map_of_accounts->end()){

        if(it->second->id == account_id){

            if(it->second->vallid_password(password)){
                it->second->deposit(amount);
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << this->id << ": Account "<< account_id << " new balance is " << it->second->balance  <<  "after " << amount << "$ was deposited" << endl;
                pthread_mutex_unlock(log_print_lock);
                pthread_mutex_lock(log_print_lock);
                return true;
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id" << account_id << " is incorrect" << endl;
                pthread_mutex_unlock(log_print_lock);
                return false;
            }
        }
        it++;
    } 

    pthread_mutex_lock(log_print_lock);
    *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id" << account_id << " is incorrect" << endl;
    pthread_mutex_unlock(log_print_lock);
    return false;
}

bool atm::withdrawl_from_account(int account_id, int password, int amount){
    map <int, account*>::iterator it;
    pthread_mutex_lock(accounts_lock);
    sleep(ACTION_SLEEP_TIME);
    it = map_of_accounts->begin();
    while(it != map_of_accounts->end()){

        if(it->second->id == account_id){

            if(it->second->vallid_password(password)){

                if(it->second->balance >= amount){
                    it->second->withdrawl(amount);
                    pthread_mutex_lock(log_print_lock);
                    *log_txt_ptr << this->id << ": Account "<< account_id << " new balance is " << it->second->balance  <<  "after " << amount << "$ was withdrew" << endl;
                    pthread_mutex_unlock(log_print_lock);
                    pthread_mutex_lock(log_print_lock);
                    return true;       
                }

                else{
                    pthread_mutex_lock(log_print_lock);
                    *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id" << account_id << " balance is lower than " << amount << endl;
                    pthread_mutex_unlock(log_print_lock);
                    return false;
                }  
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id" << account_id << " is incorrect" << endl;
                pthread_mutex_unlock(log_print_lock);
                return false;
            }
        }
        it++;
    } 

    pthread_mutex_lock(log_print_lock);
    *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id" << account_id << " is incorrect" << endl;
    pthread_mutex_unlock(log_print_lock);
    return false;
}

bool atm::closing_account(int account_id, int password){
    map <int, account*>::iterator it;
    pthread_mutex_lock(accounts_lock);
    sleep(ACTION_SLEEP_TIME);
    it = map_of_accounts->begin();
    while(it != map_of_accounts->end()){
        if(it->second->id == account_id){
            if(it->second->vallid_password(password)){
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << this->id << ": Account "<< account_id << " is now closed. Balance was" << it->second->balance  << endl;
                map_of_accounts->erase(it);
                it->second->~account();
                pthread_mutex_unlock(log_print_lock);
                pthread_mutex_unlock(accounts_lock);
                return true;
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id "<< account_id << " is incorrect" << endl;
                pthread_mutex_unlock(log_print_lock);
                pthread_mutex_unlock(accounts_lock);
                return false;
            }
        }
        it++;
    }

    pthread_mutex_lock(log_print_lock);
    *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id "<< account_id << " is incorrect" << endl;
    pthread_mutex_unlock(log_print_lock);
    pthread_mutex_unlock(accounts_lock);
    return false;
}

void atm::parser_file(){
    ifstream input_file(this->file_path);
    string current_line;
    while(input_file >> current_line){
        char delimiters[] = " ";
        char *token;

        token = strtok(&current_line[0], delimiters);
        while(token != NULL){
            switch(*token){

                //open an account
                case 'O':
                    int args[3];
                    int i=0;
                    while(token != NULL && i<3){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->create_account(args[0], args[1], args[2]);
                    break;

                //deposite to an account
                case 'D':
                    int args[3];
                    int i=0;
                    while(token != NULL && i < 3){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->deposit_to_account(args[0], args[1], args[2]);
                    break;


                //winthdrawl from an account
                case 'W':
                    int args[3];
                    int i=0;
                    while(token != NULL && i < 3){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->create_account(args[0], args[1], args[2]);
                    break;

                //check balance from an account
                case 'B':
                    int args[2];
                    int i=0;
                    while(token != NULL && i < 2){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->balance_check(args[0], args[1]);
                    break;

                //close an account
                case 'Q':
                    int args[2];
                    int i=0;
                    while(token != NULL && i < 2){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->closing_account(args[0], args[1]);
                    break;
                }
                     
        usleep(ATM_SLEEP_TIME);
        }
        
    return;
    }
}

