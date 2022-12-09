#include "account.h"
#include "atm.h"
#include <list>

#define COMMISION_TIME 3
#define SUCCESS 0

using namespace std;

void main(int argc, string argv[]){
    list<account>* list_account;
    pthread_mutex_t* log_print_lock;
    pthread_mutex_t* accounts_lock;
    int num_of_threads = argc;
    int result;
    pthread_t threads[num_of_threads];
    for(int i=0; i<argc; i++){
        pthread_create(&threads[i], NULL, (void *)&i); 




    }





}
