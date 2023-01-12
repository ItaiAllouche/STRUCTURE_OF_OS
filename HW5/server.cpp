#include "server.h"

void send_error(int sockfd, u_short error_code, string error_message, SOCK_ADDR* client_addr, socklen_t client_addr_len){

    //initiliztion of error packet
    ERROR error = {0};
    error.opcode = htons(ERROR_OPCODE);
    error.error_code = htons(error_code);
    error.error_message = error_message;

    if(sendto(sockfd, &error, sizeof(error), 0, (struct sockaddr*) client_addr, client_addr_len) < 0){
        sys_call_error("TTFTP_ERROR: sendto function error");
    }
    return;
}
