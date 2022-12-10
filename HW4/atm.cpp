#include "atm.h"

atm::atm(uint id, string file_path, map <int,account*>* map_of_accounts, pthread_mutex_t* log_print_lock, pthread_mutex_t* accounts_lock, ofstream* log_txt_ptr){
    this->id = id;
    this->map_of_accounts = map_of_accounts;
    this->file_path = file_path;
    pthread_mutex_init(&(*log_print_lock), NULL);
    pthread_mutex_init(&(*accounts_lock), NULL);
    this->log_txt_ptr = log_txt_ptr;
}

bool atm::create_account(int account_id, int balance, int password){
    if(map_of_accounts->find(account_id) == map_of_accounts->end()){
        account* new_account = new account(account_id, balance, password);
        map_of_accounts->insert(pair<int, account*>(account_id, new_account));

        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << this->id << ": New account id is " <<  account_id << " with password " << password << " and initial balance " << balance << endl;
        pthread_mutex_unlock(log_print_lock);
        return true;

    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << account_id << ": Your transaction failed - account with the same id exists" << endl;
        pthread_mutex_unlock(log_print_lock);
        return false;
    }
}

bool atm::deposit_to_account(int account_id, int password, int amount){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);
    it->second->lock_for_write();

    if(it != map_of_accounts->end()){
        if(it->second->vallid_password(password)){
            it->second->deposit(amount);

            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " new balance is " << it->second->balance  <<  "after " << amount << "$ was deposited" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return true;

        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id" << account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return false;
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        return false;
    }
}

bool atm::withdrawl_from_account(int account_id, int password, int amount){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);
    it->second->lock_for_write();
    if(it != map_of_accounts->end()){
        
        if(it->second->vallid_password(password)){

            if(it->second->withdrawl(amount)){
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << this->id << ": Account "<< account_id << " new balance is " << it->second->balance  <<  "after " << amount << "$ was withdrew" << endl;
                pthread_mutex_unlock(log_print_lock);
                it->second->unlock_from_write();
                return true;  
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id" << account_id << " balance is lower than " << amount << endl;
                pthread_mutex_unlock(log_print_lock);
                it->second->unlock_from_write();
                return false;
            }
        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return false;
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id" << account_id << " is incorrect" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        return false;
    }
}

bool atm::closing_account(int account_id, int password){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);
    it->second->lock_for_write();
    if(it != map_of_accounts->end()){

        if(it->second->vallid_password(password)){

            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " is now closed. Balance was" << it->second->balance  << endl;
            map_of_accounts->erase(it);
            it->second->~account();
            delete it->second;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return true;
        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id "<< account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return false; 
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        return false; 
    }
}

void atm::balance_check(int account_id, int password){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);
    it->second->lock_for_readres();
    if(it != map_of_accounts->end()){
        if(it->second->vallid_password(password)){
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " balance is " << it->second->balance << endl;
             pthread_mutex_unlock(log_print_lock);
             it->second->unlock_from_read();
             return ;
        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id" << account_id << " is incorrect" << endl;
             pthread_mutex_unlock(log_print_lock);
             it->second->unlock_from_read();
             return;
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
             pthread_mutex_unlock(log_print_lock);
             it->second->unlock_from_read();
             return;
    }
}

void atm::transfer_between_accounts(int src_account_id, int src_password, int dest_account_id, int dest_password, int amount){
    map<int,account*>::iterator it_src = map_of_accounts->find(src_account_id);
    map<int,account*>::iterator it_dest = map_of_accounts->find(src_account_id);
    it_src->second->lock_for_write();
    it_dest->second->lock_for_write();

    if(it_src != map_of_accounts->end() && it_dest != map_of_accounts->end()){




    }

    else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id" << it_dest->second->id<< " does not exist" << endl;
             pthread_mutex_unlock(log_print_lock);
            it_src->second->unlock_from_write();
            it_dest->second->unlock_from_write();
            return;
    }


}



/*void transfer_between_accounts(int src_account_id, int src_password, int dest_account_id, int amount){
    map<int,account*>::iterator itr_src;
    map<int,account*>::iterator itr_dest;
    itr_src = this->map_of_accounts.find(src_account_id);
    itr_dest = this->map_of_accounts.find(src_account_id);

    if(check_password_and_exist(src_account_id, src_password, itr_src) == false || itr_dest == this->map_of_accounts.end()){ //src- check password and exist 
        return false;                                                                                                        //dest-check only exist
    }

    if(itr_src->second->get_balance() < amount){    //check if the account has enough money to withdrawl
            pthread_mutex_lock(this->log_print_lock);
            *log_print_lock << "Error" << this->id << ": Your tranaction failed - account id " << account_id << " balance is lower than" << amount << endl;
            pthread_mutex_unlock(this->log_print_lock);
            return false;
    }

    itr_src->second->withdrawl(amount);
    itr_dest->second->deposit(amount);
    pthread_mutex_lock(this->log_print_lock);
    *log_print_lock << this->id << ": Transfer " << amount << " from account " << src_account << " to account " << dest_account_id << " new account balance is " << itr_src->second->balance << " new target account balance is " << itr_dest->second->balance << endl;
    pthread_mutex_unlock(this->log_print_lock);
    return true;
    
}
*/

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

