#ifndef _SERVER_H
#define _SERVER_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <stdbool.h>

#define MAX_CLIENTS 1
#define NUM_OF_ARGS 4
#define DATA_SIZE 512
#define PACKET_SIZE 516
#define ERROR_OPCODE 1
#define WRQ_OPCODE 2
#define DATA_OPCODE 3
#define ACK_OPCODE 4
#define INVALID_BLOCK_NUM 0
#define REACHED_MAX_RESENDS 0
#define WRQ_FROM_SERVED_CLIENT 4
#define FILE_ALREADAY_EXSITS 6
#define DATA_BEFORE_WRQ 7

typedef unsigned char u_char;
typedef unsigned short int u_short_int;
typedef short int short_int;
typedef unsigned long u_long;

using namespace std;

struct ACK{
    u_short opcode;
    u_short block_number;
} __attribute__((packed));

struct WRQ{
    u_short opcode;
    string file_name;
} __attribute__((packed));

struct ERROR{
    u_short opcode;
    u_short error_code;
    string error_message;
} __attribute__((packed));


struct BUFFER{
    u_short opcode;
    u_short block_number;
    char data[DATA_SIZE];
} __attribute__((packed));

struct SOCK_ADDR{
    short_int family; // AF_INET
    u_short_int port;   //port number
    u_long address; //Ip 
    u_char zero[8]; //for allignments
} __attribute__((packed));

// itay
void wrq_parser(WRQ wrq, BUFFER* buff);

// itay
void send_ack(int sockfd, u_short block_number, SOCK_ADDR* client_addr, socklen_t client_addr_len);

// itai
void send_error(int sockfd, u_short error_code, string error_message, SOCK_ADDR* client_addr, socklen_t client_addr_len);

// itai
void recieving_data_mode(int sockfd, BUFFER* buff, size_t buff_len, FILE* file_ptr, short_int timeout,
                         SOCK_ADDR* client_addr, socklen_t client_addr_lenshort_int, short_int max_num_of_resends);
// use when syscall fails.
void sys_call_error(const char* error_message){
    perror(error_message);
    exit(1);
}

#endif 