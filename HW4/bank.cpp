#include "bank.h"

bank::bank(){
    this->balance = 0;
    this->status = ACTIVE;
    this->list_of_atms = new list<atm>;
    this->map_of_accounts = new map<int,account*>;
    this->map_of_deleted_accounts = new map<int,account*>;
    this->log_txt_ptr.open("log.txt", ios::out);
    this->log_print_lock = NULL
    pthread_mutex_init(this->log_print_lock, NULL);
    this->create_lock = NULL;
    pthread_mutex_init(create_lock, NULL);
}

bank::~bank(){
    this->list_of_atms->clear();
    this->status = TERMINATED;
    this->map_of_accounts->clear();
    this->map_of_deleted_accounts->clear();
    this->log_txt_ptr.close();    
    pthread_mutex_destroy(this->log_print_lock);
    pthread_mutex_destroy(this->create_lock);
}

void bank::fee_collection(){



}

void bank::accounts_status_print(){
    while(this->status){
        usleep(ACCOUNTS_PRINT_TIME);
        map <int, account*>::iterator it = map_of_accounts->begin();
        
        while(it != map_of_accounts->end){
            cout << "Account " << it->second->id << ": Balance - " << it->second->balance << " $, Account Password - " << it->second->password << endl;
            it++;  
        }
        cout << "The Bank has "<< this->balance << " $" << endl;
    }
    return;   
}

