#include "atm.h"

atm::atm(uint id, string file_path, map <int,account*>* map_of_accounts, map <int,account*>* map_of_deleted_accounts, pthread_mutex_t* log_print_lock,
        pthread_mutex_t* create_lock, ofstream* log_txt_ptr){
    this->id = id;
    this->file_path = file_path;
    this->map_of_accounts = map_of_accounts;
    this->map_of_deleted_accounts = map_of_deleted_accounts; 
    this->log_print_lock = log_print_lock;
    this->create_lock = create_lock;
    this->log_txt_ptr = log_txt_ptr;
}

void atm::create_account(int account_id, int password, int balance){
    pthread_mutex_lock(create_lock);
    sleep(ACTION_SLEEP_TIME);
    map <int, account*>::iterator it = map_of_accounts->find(account_id);
    //the account is not exsit
    if(it == map_of_accounts->end() || it->second->deleted ){
        account* new_account = new account(account_id, balance, password);
        new_account->lock_for_write();
        map_of_accounts->insert(pair<int, account*>(account_id, new_account));
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << this->id << ": New account id is " <<  account_id << " with password " << password << " and initial balance " << balance << endl;
        new_account->unlock_from_write();
        pthread_mutex_unlock(log_print_lock);
        pthread_mutex_unlock(create_lock);
        return;
    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << account_id << ": Your transaction failed - account with the same id exists" << endl;
        pthread_mutex_unlock(log_print_lock);
        pthread_mutex_unlock(create_lock);
        return;
    }
}

void atm::deposit_to_account(int account_id, int password, int amount){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
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
            return;

        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return;
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        return;
    }
}

void atm::withdrawl_from_account(int account_id, int password, int amount){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
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
                return;  
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id" << account_id << " balance is lower than " << amount << endl;
                pthread_mutex_unlock(log_print_lock);
                it->second->unlock_from_write();
                return;
            }
        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return;
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        return;
    }
}

void atm::closing_account(int account_id, int password){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        return;
    }

    it->second->lock_for_write();

    if(it != map_of_accounts->end() && !it->second->deleted){

        if(it->second->vallid_password(password)){
            map_of_deleted_accounts->insert(pair<int, account*>(it->second->id, it->second));
            it->second->deleted = true;
            map_of_accounts->erase(it);
            
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " is now closed. Balance was " << it->second->balance  << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return;
        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id "<< account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it->second->unlock_from_write();
            return; 
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        it->second->unlock_from_write();
        return; 
    }
}

void atm::balance_check(int account_id, int password){
    map <int, account*>::iterator it = map_of_accounts->find(account_id);

    //the account has never been created
    if(it == map_of_accounts->end()){
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        return;
    }

    it->second->lock_for_readres();

    if(it != map_of_accounts->end() && !it->second->deleted){
        if(it->second->vallid_password(password)){
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << this->id << ": Account "<< account_id << " balance is " << it->second->balance << endl;
             pthread_mutex_unlock(log_print_lock);
             it->second->unlock_from_read();
             return;
        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << account_id << " is incorrect" << endl;
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

void atm::transfer_between_accounts(int src_account_id, int src_password, int dest_account_id, int amount){
    map<int,account*>::iterator it_src = map_of_accounts->find(src_account_id);
    map<int,account*>::iterator it_dest = map_of_accounts->find(dest_account_id);

    //the src_account has never been created
    if(it_src == map_of_accounts->end()){
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << src_account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        return;
    }

    //the dest_account has never been created
    if(it_src == map_of_accounts->end()){
        pthread_mutex_lock(log_print_lock);
        *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << dest_account_id << " does not exist" << endl;
        pthread_mutex_unlock(log_print_lock);
        return;
    }

    it_src->second->lock_for_write();
    it_dest->second->lock_for_write();

    if((it_src != map_of_accounts->end()  && !it_src->second->deleted) && (it_dest != map_of_accounts->end() && !it_dest->second->deleted )){
        if(it_src->second->vallid_password(src_password)){
            
            if(it_src->second->withdrawl(amount)){
                it_dest->second->deposit(amount);;

                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr <<  this->id << ": Transfer " << amount << " from account " << src_account_id  << " to account " << dest_account_id \
                << " new account balance is " << it_src->second->balance <<  " new target account balance is " << it_dest->second->balance << endl;
                pthread_mutex_unlock(log_print_lock);
                it_src->second->unlock_from_write();
                it_dest->second->unlock_from_write();
                return;
            }

            else{
                pthread_mutex_lock(log_print_lock);
                *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << src_account_id << " balance is lower than " << amount << endl;
                pthread_mutex_unlock(log_print_lock);
                it_src->second->unlock_from_write();
                it_dest->second->unlock_from_write();
                return;
            }

        }

        else{
            pthread_mutex_lock(log_print_lock);
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - password for account id " << src_account_id << " is incorrect" << endl;
            pthread_mutex_unlock(log_print_lock);
            it_src->second->unlock_from_write();
            it_dest->second->unlock_from_write();
            return;  
        }
    }

    else{
        pthread_mutex_lock(log_print_lock);
        //src account wasnt found
        if(it_src == map_of_accounts->end() || it_src->second->deleted ){
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " <<src_account_id << " does not exist" << endl;
        }
        //dest account wasnt found
        if(it_dest == map_of_accounts->end() || it_dest->second->deleted ){
            *log_txt_ptr << "Error " << this->id << ": Your transaction failed - account id " << dest_account_id << " does not exist" << endl;
        }
        
        pthread_mutex_unlock(log_print_lock);
        it_src->second->unlock_from_write();
        it_dest->second->unlock_from_write();
        return;
    }
}

void atm::parser_file(){
    ifstream input_file(this->file_path.c_str());
    string current_line;
    int args[4];
    int i = 0;

    while(getline(input_file,current_line)){
        char delimiters[] = " ";
        char *token;

        token = strtok(&current_line[0], delimiters);
        while(token != NULL){
            switch(*token){

                //open an account
                case 'O':
                    i = 0;
                    while(token != NULL && i<3){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->create_account(args[0], args[1], args[2]); 
                    break;

                //deposit to an account
                case 'D':
                    i = 0;     
                    while(token != NULL && i<3){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->deposit_to_account(args[0], args[1], args[2]);
                    break;

                //withdrawl from an account
                case 'W':
                    i = 0;             
                    while(token != NULL && i < 3){
                    token = strtok( NULL, delimiters);
                    args[i] = atoi(token);
                    i++;
                    }
                    this->withdrawl_from_account(args[0], args[1], args[2]);
                    break;

                //check account balance
                case 'B':
                    i = 0;
                    while(token != NULL && i < 2){
                    token = strtok( NULL, delimiters);
                    args[i] = atoi(token);
                    i++;
                    }
                    this->balance_check(args[0], args[1]);
                    break;

                //close account
                case 'Q': 
                    i = 0; 
                    while(token != NULL && i < 2){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->closing_account(args[0], args[1]);
                    break;

                //transfer between accounts
                case 'T':
                    i = 0;
                    while(token != NULL && i < 4){
                        token = strtok( NULL, delimiters);
                        args[i] = atoi(token);
                        i++;
                    }
                    this->transfer_between_accounts(args[0], args[1], args[2], args[3]);
                    break;
            }
            
            usleep(ATM_SLEEP_TIME);
            break;
        }
   
    }
    return;
}


