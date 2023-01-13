#ifndef _SERVER_H
#define _SERVER_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>

#define SUCCESS 0
#define MAX_CLIENTS 1
#define OPCODE_SIZE 2
#define NUM_OF_ARGS 4
#define DATA_SIZE 512
#define PACKET_SIZE 516

//opcode defines
#define ERROR_OPCODE 1
#define WRQ_OPCODE 2
#define DATA_OPCODE 3
#define ACK_OPCODE 4

//error opcode
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
    char file_name [DATA_SIZE];
    char trans_mode [DATA_SIZE];
} __attribute__((packed));

struct ERROR{
    u_short opcode;
    u_short error_code;
    char error_message [DATA_SIZE];
} __attribute__((packed));


struct BUFFER{
    u_short opcode;
    u_short block_number;
    char data[DATA_SIZE];
} __attribute__((packed));

/*
struct SOCK_ADDR{
    short_int family; // AF_INET
    u_short_int port;   //port number
    u_long address; //Ip 
    u_char zero[8]; //for allignments
} __attribute__((packed));
*/

// in charge of validate the first packet which sent and open the requested file
FILE* wrq_parser(int sockfd, struct sockaddr_in* client, socklen_t client_addr_len, WRQ* wrq);

void send_ack(int sockfd, u_short block_number, struct sockaddr_in* client_addr, socklen_t client_addr_len);

void send_error(int sockfd, u_short error_code, const char* error_message, struct sockaddr_in* client_addr, socklen_t client_addr_len);

void recieving_data_mode(int sockfd, BUFFER* buff, size_t buff_len, FILE* file_ptr, short_int timeout,
                         struct sockaddr_in* client_addr, socklen_t client_addr_len, short_int max_num_of_resends);

// use when syscall fails.
void sys_call_error(const char* error_message);

#endif 