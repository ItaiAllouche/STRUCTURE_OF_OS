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

void account::deposit(int amount){
    sleep(ACTION_SLEEP_TIME);
    this->balance += amount;
}

bool account::withdrawl(int amount){
    sleep(ACTION_SLEEP_TIME);
    if(this->balance < amount){
        return false;
    }
    this->balance -= amount;
    return true;
 }

void account::lock_for_write(){
    pthread_mutex_lock(&lock_write);   
}

void account::unlock_from_write(){
   pthread_mutex_unlock(&lock_write); 
}

void account::lock_for_readres(){
    pthread_mutex_lock(&lock_read);
    this->num_of_readers++;

    //first reader
    if(num_of_readers == 1){
        pthread_mutex_lock(&lock_write);
    }
    pthread_mutex_unlock(&lock_read);
}

void account::unlock_from_read(){
    pthread_mutex_lock(&lock_read);
    this->num_of_readers--;
     
     //last reader
    if(num_of_readers == 0){
        pthread_mutex_unlock(&lock_write);
    }
    pthread_mutex_unlock(&lock_read);
}