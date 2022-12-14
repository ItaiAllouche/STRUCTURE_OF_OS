#include "bank.h"

bank::bank(){
    this->balance = 0;
    this->status = ACTIVE;
    this->list_of_atms = new list<atm*>;
    //this->map_of_accounts = new  map<int,account*>;
    //this->map_of_deleted_accounts = new map<int,account*>;
    this->log_txt_ptr.open("log.txt,", ios::out);
    //this->log_print_lock = NULL;
    pthread_mutex_init(log_print_lock, NULL);
    this->create_lock = NULL;
    pthread_mutex_init(create_lock, NULL);
}

bank::~bank(){
    this->balance = TERMINATED;
    delete[] this->list_of_atms;
    this->log_txt_ptr.close();
    pthread_mutex_destroy(this->log_print_lock);
    pthread_mutex_destroy(this->create_lock);
}

void bank::accounts_status_print(){
    while(this->status){
        usleep(ACCOUNTS_PRINT_TIME);
        printf("\033[2J");
        printf("\033[1;1H");
        cout << "Current Bank Status" << endl;

        map <int, account*>::iterator it = map_of_accounts.begin();
        while(it != map_of_accounts.end()){
            it->second->lock_for_readres();
            cout << "Account " << it->second->id << ": Balance - " << it->second->balance << " $, Account Password - " << it->second->password << endl;
            it->second->unlock_from_read();
            it++;  
        }
        cout << "The Bank has "<< this->balance << " $" << endl;
    }
    return;   
}

void bank::fee_collection(){
    while(this->status){
        sleep(FEE_TIME);
        int rand_fee = rand() % MAX_FEE + 1;

        map <int, account*>::iterator it = map_of_accounts.begin();
        while(it != map_of_accounts.end()){
            it->second->lock_for_write();
            int fee = (int) round((double)it->second->balance * (rand_fee / PERCENTAGE));
            this->balance += fee;
            it->second->balance -= fee;
            it->second->unlock_from_write();
            it++;
        }
    }
    return;
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
    for(int i=1; i < argc-1; i++){
        string curr_file = argv[i];
        if(vallid_file(curr_file)){
            arr_of_files.push_back(curr_file);
        }

        else{
            cerr << "Bank error: illegal arguments" << endl;;
        exit(1);
        }
    }

    // initiate thread for each file, each thread execute atm_handler function
    pthread_t* atm_threads = new pthread_t[arr_of_files.size()];
    bank* leumi = new bank();
    atm* curr_atm;

    for(uint i=0; i<arr_of_files.size(); i++){
        curr_atm = new atm(i,arr_of_files[i],&(leumi->map_of_accounts),&(leumi->map_of_deleted_accounts),leumi->log_print_lock,leumi->create_lock,&(leumi->log_txt_ptr));

        if(pthread_create(&atm_threads[i],NULL,atm_handler,(void*)curr_atm) != 0){
            perror("Bank error: pthread_create failed");
        }
        leumi->list_of_atms->push_back(curr_atm);
    }

    //initiate fee collection thread - in chrage of collect fees from accounts every 3 sec. exectue by fee_collection_handler
    pthread_t fee_collection_thread;
    if(pthread_create(&fee_collection_thread,NULL,fee_collection_handler,(void*)leumi) != 0){
        perror("Bank error: pthread_create failed");   
    }

    //initiate account print thread - in chrage of print current balance of accounts every 0.5 sec. execute by accounts_print_handler
    pthread_t accounts_print_thread;
    if(pthread_create(&accounts_print_thread,NULL,accounts_print_handler,(void*)leumi) != 0){
        perror("Bank error: pthread_create failed");   
    }

    //wait for atm threads to terminate
    for(uint i=0; i<arr_of_files.size(); i++){
        if(pthread_join(atm_threads[i],NULL) != 0){
            perror("Bank error: pthread_join failed");
        }      
    }

    //terminate leumi bank
    leumi->status = TERMINATED;

    //wait for fee collection thread to terminate
    if(pthread_join(fee_collection_thread,NULL) != 0){
        perror("Bank error: pthread_join failed");
    }

    //wait for accounts print thread to terminate
    if(pthread_join(accounts_print_thread,NULL) != 0){
        perror("Bank error: pthread_join failed");
    }

    delete[] leumi;

    return 0;   
}