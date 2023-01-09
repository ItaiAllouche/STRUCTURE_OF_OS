#include "atm.h"

extern bool accounts_print;
extern bool stdout_print_lock_is_taken;
extern pthread_mutex_t stdout_print_lock;

atm::atm(uint id, string file_path, map<int,account*>* map_of_accounts, map<int,account*>* map_of_deleted_accounts, int* account_list_num_of_readers,
        pthread_mutex_t* log_print_lock, pthread_mutex_t* transfer_lock, pthread_mutex_t* account_list_read_lock,
        pthread_mutex_t* account_list_write_lock, ofstream* log_txt_ptr){
    this->id = id;
    this->file_path = file_path;
    this->map_of_accounts = map_of_accounts;
    this->map_of_deleted_accounts = map_of_deleted_accounts; 
    this->account_list_num_of_readers = account_list_num_of_readers;
    this->log_print_lock = log_print_lock;
    this->create_lock = create_lock;
    this->transfer_lock = transfer_lock;
    this->account_list_read_lock = account_list_read_lock;
    this->account_list_write_lock = account_list_write_lock;
    this->log_txt_ptr = log_txt_ptr;
}

void atm::lock_account_list_for_read(){
    pthread_mutex_lock(this->account_list_read_lock);
    *this->account_list_num_of_readers = *this->account_list_num_of_readers + 1;

    //first reader
    if(*account_list_num_of_readers == 1){
        pthread_mutex_lock(this->account_list_write_lock);
    }
    pthread_mutex_unlock(this->account_list_read_lock);
}

void atm::unlock_account_list_from_read(){
    pthread_mutex_lock(this->account_list_read_lock);
    *this->account_list_num_of_readers = *this->account_list_num_of_readers - 1;
     
     //last reader
    if(*account_list_num_of_readers == 0){
        pthread_mutex_unlock(this->account_list_write_lock);
    }
    pthread_mutex_unlock(this->account_list_read_lock);
}

void atm::create_account(int account_id, string password, int balance){

    //pthread_mutex_lock(create_lock);

    accounts_print = true;
    pthread_mutex_lock(this->account_list_write_lock);

    map <int, account*>::iterator it = map_of_accounts->find(account_id);
    //the account does not exsit
    if(it == map_of_accounts->end() || it->second->deleted ){
        account* new_account = new account(account_id, balance, password);
        new_account->lock_for_write();
        sleep(ACTION_SLEEP_TIME);
        map_of_accounts->insert(pair<int, account*>(account_id, new_account));
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << this->id << ": New account id is " <<  account_id << " with password " << password << " and initial balance " << balance << endl;
        pthread_mutex_unlock(log_print_lock);
        new_account->unlock_from_write();

        pthread_mutex_unlock(this->account_list_write_lock);
        accounts_print = false;
        pthread_mutex_unlock(&stdout_print_lock);
        //if(stdout_print_lock_is_taken){
            //pthread_mutex_unlock(&stdout_print_lock);
            //stdout_print_lock_is_taken = false;
        //}
        return;
    }

    else{
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << account_id << ": Your transaction failed - account with the same id exists" << endl;
        pthread_mutex_unlock(log_print_lock);
        
        pthread_mutex_unlock(this->account_list_write_lock);
        accounts_print = false;
        pthread_mutex_unlock(&stdout_print_lock);
        //if(stdout_print_lock_is_taken){
            //pthread_mutex_unlock(&stdout_print_lock);
            //stdout_print_lock_is_taken = false;
        //}
        return;
    }
}

void atm::deposit_to_account(int account_id, string password, int amount){

    //if a writer wants to write in map_of_acounts -> go to sleep
    if(accounts_print){
        pthread_mutex_lock(&stdout_print_lock);
        stdout_print_lock_is_taken = true;
    }
    this->lock_account_list_for_read();
    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        this->unlock_account_list_from_read();
        return;
    }

    it->second->lock_for_write();

    if(it != map_of_accounts->end() && !it->second->deleted){
        if(it->second->vallid_password(password)){
            it->second->deposit(amount);

            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " new balance is " << it->second->balance  <<  " after " << amount << "$ was deposited" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            this->unlock_account_list_from_read();
            return;

        }

        else{
            sleep(ACTION_SLEEP_TIME);
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            this->unlock_account_list_from_read();
            return;
        }
    }

    else{
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        this->unlock_account_list_from_read();
        return;
    }
}

void atm::withdrawl_from_account(int account_id, string password, int amount){

    //if a writer wants to write in map_of_acounts -> go to sleep
    if(accounts_print){
        pthread_mutex_lock(&stdout_print_lock);
        stdout_print_lock_is_taken = true;
    }
    this->lock_account_list_for_read();
    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        this->unlock_account_list_from_read();
        return;
    }

    it->second->lock_for_write();

    if(it != map_of_accounts->end() && !it->second->deleted){
        
        if(it->second->vallid_password(password)){

            if(it->second->withdrawl(amount)){
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << this->id << ": Account "<< account_id << " new balance is " << it->second->balance  <<  " after " << amount << "$ was withdrew" << endl;
                pthread_mutex_unlock(log_print_lock);
                it->second->unlock_from_write();
                this->unlock_account_list_from_read();
                return;  
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " balance is lower than " << amount << endl;
                pthread_mutex_unlock(log_print_lock);
                it->second->unlock_from_write();
                this->unlock_account_list_from_read();
                return;
            }
        }

        else{
            sleep(ACTION_SLEEP_TIME);
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            this->unlock_account_list_from_read();
            return;
        }
    }

    else{
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        this->unlock_account_list_from_read();
        return;
    }
}

void atm::closing_account(int account_id, string password){
    accounts_print = true;
    pthread_mutex_lock(this->account_list_write_lock);

    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);

        pthread_mutex_unlock(this->account_list_write_lock);
        accounts_print = false;
        //pthread_mutex_unlock(&stdout_print_lock);
        if(stdout_print_lock_is_taken){
            pthread_mutex_unlock(&stdout_print_lock);
            stdout_print_lock_is_taken = false;
        }
        return;
    }

    it->second->lock_for_write();

    if(it != map_of_accounts->end() && !it->second->deleted){

        if(it->second->vallid_password(password)){
            sleep(ACTION_SLEEP_TIME);
            map_of_deleted_accounts->insert(pair<int, account*>(it->second->id, it->second));
            it->second->deleted = true;
            map_of_accounts->erase(it);
            
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " is now closed. Balance was " << it->second->balance  << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();

            pthread_mutex_unlock(this->account_list_write_lock);
            accounts_print = false;
            pthread_mutex_unlock(&stdout_print_lock);
            //if(stdout_print_lock_is_taken){
                //pthread_mutex_unlock(&stdout_print_lock);
                //stdout_print_lock_is_taken = false;
            //}
            return;
        }

        else{
            sleep(ACTION_SLEEP_TIME);
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id "<< account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();

            pthread_mutex_unlock(this->account_list_write_lock);
            accounts_print = false;
            pthread_mutex_unlock(&stdout_print_lock);
            //if(stdout_print_lock_is_taken){
                //pthread_mutex_unlock(&stdout_print_lock);
                //stdout_print_lock_is_taken = false;
            //}
            return; 
        }
    }

    else{
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        
        pthread_mutex_unlock(this->account_list_write_lock);
        accounts_print = false;
        pthread_mutex_unlock(&stdout_print_lock);
        //if(stdout_print_lock_is_taken){
            //pthread_mutex_unlock(&stdout_print_lock);
            //stdout_print_lock_is_taken = false;
            //}
        return; 
    }
}

void atm::balance_check(int account_id, string password){
    
    //if a writer wants to write in map_of_acounts -> go to sleep
    if(accounts_print){
        pthread_mutex_lock(&stdout_print_lock);
        stdout_print_lock_is_taken = true;
    }
    this->lock_account_list_for_read();
    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        this->unlock_account_list_from_read();
        return;
    }

    it->second->lock_for_readres();

    if(it != map_of_accounts->end() && !it->second->deleted){
        if(it->second->vallid_password(password)){
            sleep(ACTION_SLEEP_TIME);
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " balance is " << it->second->balance << endl;
             pthread_mutex_unlock(log_print_lock);
             it->second->unlock_from_read();
             this->unlock_account_list_from_read();
             return;
        }

        else{
            sleep(ACTION_SLEEP_TIME);
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << endl;
             pthread_mutex_unlock(log_print_lock);
             it->second->unlock_from_read();
             this->unlock_account_list_from_read();
             return;
        }
    }

    else{
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
             pthread_mutex_unlock(log_print_lock);
             it->second->unlock_from_read();
             this->unlock_account_list_from_read();
             return;
    }
}

void atm::transfer_between_accounts(int src_account_id, string src_password, int dest_account_id, int amount){
    
    //if a writer wants to write in map_of_acounts -> go to sleep
    if(accounts_print){
        pthread_mutex_lock(&stdout_print_lock);
        stdout_print_lock_is_taken = true;
    }
    this->lock_account_list_for_read();
    pthread_mutex_lock(this->transfer_lock);
    map<int,account*>::iterator it_src = map_of_accounts->find(src_account_id);
    map<int,account*>::iterator it_dest = map_of_accounts->find(dest_account_id);

    //the src_account has never been created
    if(it_src == map_of_accounts->end()){
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << src_account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        pthread_mutex_unlock(this->transfer_lock);
        this->unlock_account_list_from_read();
        return;
    }

    //the dest_account has never been created
    if(it_dest == map_of_accounts->end()){
        sleep(ACTION_SLEEP_TIME);
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << dest_account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        pthread_mutex_unlock(this->transfer_lock);
        this->unlock_account_list_from_read();
        return;
    }

    it_src->second->lock_for_write();
    it_dest->second->lock_for_write();

    if(it_src != map_of_accounts->end() && !it_src->second->deleted && it_dest != map_of_accounts->end() && !it_dest->second->deleted){
        if(it_src->second->vallid_password(src_password)){
            
            if(it_src->second->withdrawl(amount)){
                it_dest->second->balance = it_dest->second->balance + amount;

                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr <<  this->id << ": Transfer " << amount << " from account " << src_account_id  << " to account " << dest_account_id \
                << " new account balance is " << it_src->second->balance <<  " new target account balance is " << it_dest->second->balance << endl;
                pthread_mutex_unlock(log_print_lock);
                it_src->second->unlock_from_write();
                it_dest->second->unlock_from_write();
                pthread_mutex_unlock(this->transfer_lock);
                this->unlock_account_list_from_read();
                return;
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << src_account_id << " balance is lower than " << amount << endl;
                pthread_mutex_unlock(log_print_lock);
                it_src->second->unlock_from_write();
                it_dest->second->unlock_from_write();
                pthread_mutex_unlock(this->transfer_lock);
                this->unlock_account_list_from_read();
                return;
            }
        }

        else{
            sleep(ACTION_SLEEP_TIME);
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << src_account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it_src->second->unlock_from_write();
            it_dest->second->unlock_from_write();
            pthread_mutex_unlock(this->transfer_lock);
            this->unlock_account_list_from_read();
            return;  
        }
    }

    else{
        sleep(ACTION_SLEEP_TIME);
        if(it_src == map_of_accounts->end() || it_src->second->deleted){
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << src_account_id << " does not exist" << endl;
            pthread_mutex_unlock(log_print_lock);
            it_src->second->unlock_from_write();
            it_dest->second->unlock_from_write();
            pthread_mutex_unlock(this->transfer_lock);
            this->unlock_account_list_from_read();
            return;
        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << dest_account_id << " does not exist" << endl;
            pthread_mutex_unlock(log_print_lock);
            it_src->second->unlock_from_write();
            it_dest->second->unlock_from_write();
            pthread_mutex_unlock(this->transfer_lock);
            this->unlock_account_list_from_read();
            return;
        }
    }   
}

void atm::parser_file(){
    ifstream input_file(this->file_path.c_str());
    string current_line;
    char action;
    int account_id;
    string account_password;
    int amount;
    int target_account_id;

    while(getline(input_file,current_line)){
        if(current_line.empty()){
            continue;
        }
        stringstream full_line(current_line);
        full_line >> action;
        full_line >> account_id;
        full_line >> account_password;

        switch(action){

            //open an account
            case 'O':
                full_line >> amount;
                this->create_account(account_id, account_password, amount);
                break;

            //deposit from account
            case 'D':
                full_line >> amount;
                this->deposit_to_account(account_id, account_password, amount);
                break;

            //withdrawl from account
            case 'W':
                full_line >> amount;
                this->withdrawl_from_account(account_id, account_password, amount);
                break;

            //check balance of account
            case 'B':
                this->balance_check(account_id, account_password);
                break;

            //close an account
                case 'Q':
                this->closing_account(account_id, account_password);
                break;

            //transfer between accounts
            case 'T':
                full_line >> target_account_id;
                full_line >> amount;
                this->transfer_between_accounts(account_id, account_password, target_account_id, amount);
                break;

            default:
                break;
        }
        usleep(ATM_SLEEP_TIME);
    }
    input_file.close();
    return;
}

