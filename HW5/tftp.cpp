#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

#define MAX_CLIENTS 1
#define NUM_OF_ARGS 4
#define DATA_SIZE 512
#defiine 

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long u_long;

using namespace std;


class ACK{
    u_short opcode;
    u_short block_number;
} __attribute__((packed));

class WRQ{
    u_short opcode;
    string file_name;
} __attribute__((packed));

class DATA{
    u_short opcode;
    u_short block_number;
    char data[DATA_SIZE];
} __attribute__((packed));

class ERROR{
    u_short opcode;
    u_short error_code;
    string error_message;
} __attribute__((packed));

struct SOCK_ADDR{
    u_short family; // AF_INET
    u_short port;   //port number
    u_long address; //Ip 
    u_char zero[8]; //for allignments
} __attribute__((packed));

/*struct CLIENT_ADDR{
    u_short family; // AF_INET
    u_short port;   //port number
    u_long address; //Ip 
    u_char zero[8]; //for allignments
} __attribute__((packed));*/

struct BUFFER{
    u_short opcode;
    u_short block_number;
    char data[DATA_SIZE];
}


void arg_error(){
    cerr << "TTFTP_ERROR: illegal arguments" << endl;
    exit(1);
}

void sys_call_error(const char* error_message){
    perror(error_message);
    exit(1);
}


int main(int argc, char *argv[]){
    //to be completed!************
    if(argc != NUM_OF_ARGS || atoi(argv[1]) || atoi(argv[2]) || atoi(argv[3])){
        arg_error();
    }

    int sockfd;
    int new_sockfd;
    int port_num;
    int client_addr_len;
    SOCK_ADDR my_addr = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        sys_call_error("TTFTP_ERROR: socket failed");
    }

    port_num = atoi(argv[1]);
    my_addr.family = AF_INET;
    my_addr.address = INADDR_ANY;
    my_addr.port = htons(port_num);

    if(bind(sockfd, (const struct sockaddr *)&my_addr, sizeof(my_addr))){
        sys_call_error("TTFTP_ERROR: bind failed");   
    }


    recvfrom()









}