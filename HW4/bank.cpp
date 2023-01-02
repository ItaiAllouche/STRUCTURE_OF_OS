#include "bank.h"

bool progress = true;

bank::bank(){
    this->balance = 0;
    this->list_of_atms = new list<atm*>;
    this->log_txt_ptr.open("log.txt", ios::out);
    pthread_mutex_init(&this->log_print_lock, NULL);
    pthread_mutex_init(&this->create_lock, NULL);
    pthread_mutex_init(&this->transfer_lock, NULL);
}

bank::~bank(){
    this->free_atms();
    this->log_txt_ptr.close();
    pthread_mutex_destroy(&this->log_print_lock);
    pthread_mutex_destroy(&this->create_lock);
    pthread_mutex_destroy(&this->transfer_lock);
    this->free_accounts();    
}

void bank::accounts_status_print(){
    while(progress){
        printf("\033[2J");
        printf("\033[1;1H");
        cout << "Current Bank Status" << endl;

        map <int, account*>::iterator it = map_of_accounts.begin();
        while(it != map_of_accounts.end() && it->second != 0){
            it->second->lock_for_readres();
            cout << "Account " << it->second->id << ": Balance - " << it->second->balance << " $, Account Password - " << it->second->password << endl;
            it->second->unlock_from_read();
            it++;  
        }
        cout << "The Bank has "<< this->balance << " $" << endl;
        usleep(ACCOUNTS_PRINT_TIME);
    }
    return;   
}

void bank::fee_collection(){
    while(progress){
        sleep(FEE_TIME);
        double rand_fee = rand() % MAX_FEE + 1;

        map <int, account*>::iterator it = map_of_accounts.begin();
        while(it != map_of_accounts.end() && it->second != 0){
            it->second->lock_for_write();
            double to_round = (double)it->second->balance*(rand_fee / PERCENTAGE);
            int fee = (int)round(to_round);
            this->balance += fee;
            it->second->balance -= fee;
            it->second->unlock_from_write();

            pthread_mutex_lock(&log_print_lock);
            log_txt_ptr << "Bank: commissions of " << rand_fee << " % were charged, the bank gained " << fee << " $ from account "<< it->second->id <<  endl;
            pthread_mutex_unlock(&log_print_lock);
            it++;
        }
    }
    return;
}

void bank::free_accounts(){
    map<int,account*>::iterator it1 = this->map_of_accounts.begin();
    while(it1 != this->map_of_accounts.end()){
        map<int,account*>::iterator temp_it = it1;
        it1++;
        delete temp_it->second;
    }

    map<int,account*>::iterator it2 = this->map_of_deleted_accounts.begin();
    while(it2 != this->map_of_deleted_accounts.end()){
        map<int,account*>::iterator temp_it = it2;
        it2++;
        delete temp_it->second;
    }
}

void bank::free_atms(){
    list<atm*>::iterator it = this->list_of_atms->begin();
    while(it != this->list_of_atms->end()){
        list<atm*>::iterator temp_it = it;
        it++;
        delete *temp_it;
    }

}

void* atm_handler(void* generic_atm){
    atm* curr_atm = (atm*)generic_atm;
    curr_atm->parser_file();
    pthread_exit(NULL);
}

void* fee_collection_handler(void* bank_leumi){
    bank* curr_bank = (bank*)bank_leumi;
    curr_bank->fee_collection();
    pthread_exit(NULL);
}

void* accounts_print_handler(void* bank_leumi){
    bank* curr_bank = (bank*)bank_leumi;
    curr_bank->accounts_status_print();
    pthread_exit(NULL);
}

bool vallid_file(string file){
    ifstream res(file.c_str());
    return res.good();    
}

int main(int argc, char* argv[]){
    if(argc < 2){
        cerr << "Bank error: illegal arguments" << endl;
        exit(1);
    }

    // fill array with vallid files
    vector<string> arr_of_files;
    for(int i=1; i < argc; i++){
        string curr_file = argv[i];
        if(vallid_file(curr_file)){
            arr_of_files.push_back(curr_file);
        }

        else{
            cerr << "Bank error: illegal arguments" << endl;
            exit(1);
        }
    }
    // initiate thread for each file, each thread execute atm_handler function
    pthread_t* atm_threads = new pthread_t[arr_of_files.size()];
    bank* leumi = new bank();
    atm* curr_atm;

    for(uint i=0; i<arr_of_files.size(); i++){
        curr_atm = new atm(1+i,arr_of_files[i],&(leumi->map_of_accounts), &(leumi->map_of_deleted_accounts), &(leumi->log_print_lock), &(leumi->create_lock),\
        &(leumi->transfer_lock),&(leumi->log_txt_ptr));

        if(pthread_create(&atm_threads[i],NULL,atm_handler,(void*)curr_atm) != 0){
            perror("Bank error: pthread_create failed");
            exit(1);
        }
        leumi->list_of_atms->push_back(curr_atm);
    }

    //initiate account print thread - in chrage of print current balance of accounts every 0.5 sec. execute by accounts_print_handler
    pthread_t accounts_print_thread;
    if(pthread_create(&accounts_print_thread,NULL,accounts_print_handler,(void*)leumi) != 0){
        perror("Bank error: pthread_create failed");
        exit(1);   
    }

    //initiate fee collection thread - in chrage of collect fees from accounts every 3 sec. exectue by fee_collection_handler
    pthread_t fee_collection_thread;
    if(pthread_create(&fee_collection_thread,NULL,fee_collection_handler,(void*)leumi) != 0){
        perror("Bank error: pthread_create failed");
        exit(1);   
    }

    //wait for atm threads to terminate
    for(uint i=0; i<arr_of_files.size(); i++){
        if(pthread_join(atm_threads[i],NULL) != 0){
            perror("Bank error: pthread_join failed");
            exit(1);
        }  
    }
    //terminate leumi bank
    progress = false;

    //wait for fee collection thread to terminate
    if(pthread_join(fee_collection_thread,NULL) != 0){
        perror("Bank error: pthread_join failed");
        exit(1);
    }

    //wait for accounts print thread to terminate
    if(pthread_join(accounts_print_thread,NULL) != 0){
        perror("Bank error: pthread_join failed");
        exit(1);
    }

    delete[] atm_threads;
    delete leumi;

    return 0;   
}