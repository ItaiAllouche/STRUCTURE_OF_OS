#include "account.h"


account::account(int id, int balance, int password){
    this->id = id;
    this->balance = balance;
    this->password = password;
    this->num_of_readers = 0;
    pthread_mutex_init(&lock_read, NULL);
    pthread_mutex_init(&lock_write, NULL);
}

account::~account(){
    pthread_mutex_destroy(&lock_read);
    pthread_mutex_destroy(&lock_write);
}

bool account::vallid_password(int password){
    return (this->password = password) ? true : false;
}

int account::get_balance(){
    pthread_mutex_lock(&lock_read);
    num_of_readers++;
    sleep(ACTION_SLEEP_TIME);

    //first reader
    if(num_of_readers == 1){
        pthread_mutex_lock(&lock_write);
    }

    pthread_mutex_unlock(&lock_read);
    int output = balance;
    pthread_mutex_lock(&lock_read);
    num_of_readers--;

    //first reader
    if(num_of_readers == 0){
        pthread_mutex_unlock(&lock_write);
    }

    pthread_mutex_unlock(&lock_read);
    return output;
}

void account::deposit(int amount){
    pthread_mutex_lock(&lock_write);
    sleep(ACTION_SLEEP_TIME);
    this->balance += amount;
    pthread_mutex_unlock(&lock_write);
}

bool account::withdrawl(int amount){
    pthread_mutex_lock(&lock_write);
    sleep(ACTION_SLEEP_TIME);

    if(this->balance < amount){
        pthread_mutex_unlock(&lock_write);
        return false;
    }
    this->balance -= amount;
    pthread_mutex_unlock(&lock_write);
    return true;
}