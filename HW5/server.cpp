#include "server.h"

void sys_call_error(const char* error_message){
    perror(error_message);
    exit(1);
}

void send_error(int sockfd, u_short error_code, const char* error_message, struct sockaddr_in* client_addr, socklen_t client_addr_len){

    //initiliztion of error packet
    ERROR error = {0};
    error.opcode = htons(ERROR_OPCODE);
    error.error_code = htons(error_code);
    strcpy(error.error_message,error_message);

    if(sendto(sockfd, &error, sizeof(error), 0, (struct sockaddr*) client_addr, client_addr_len) < 0){
        sys_call_error("TTFTP_ERROR: sendto faild");
    }
    return;
}

void send_ack(int sockfd, u_short block_number, struct sockaddr_in* client_addr, socklen_t client_addr_len){
    //initiliztion of ack packet
    ACK ack = {0};
    ack.opcode = htons(ACK_OPCODE);
    ack.block_number = htons(block_number);

    if(sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr*) client_addr, client_addr_len) < 0){
        sys_call_error("TTFTP_ERROR: sendto faild");
    }
    return;
}

void recieving_data_mode(int sockfd, BUFFER* buff, size_t buff_len, FILE* file_ptr, short_int timeout, struct sockaddr_in* client_addr,
                         socklen_t client_addr_len, short_int max_num_of_resends){                        
    fd_set readfds;
    struct timeval corrent_time;
    int data_to_read = 0;
    u_short block_number = 0;
    int missed_packets = 0;
    size_t message_size = 0;

    //initialize the bit sets     
    FD_ZERO(&readfds);

    //add readfds to the appropriate bit set
    FD_SET(sockfd, &readfds);

    do{
        //initilize buffer
        memset(buff, 0, buff_len); 

        //validate the receieved data 
        do{
            //wait to read data from the socket
            while(true){
                //initilize current time struct
                corrent_time.tv_sec = timeout;
                corrent_time.tv_usec = 0;
                data_to_read = select(sockfd+1, &readfds, NULL, NULL, &corrent_time);

                if(data_to_read < 0){
                    sys_call_error("TTFTP_ERROR: select faild");
                }

                // no data was in the socket in timeout window
                else if(data_to_read == 0){
                    missed_packets++;
                    
                    // too many packets were missed - fatal error
                    if(missed_packets >= max_num_of_resends){
                        send_error(sockfd, REACHED_MAX_RESENDS, "Abandoning file transmission", client_addr, client_addr_len);
                        return;
                    }
                    send_ack(sockfd, block_number, client_addr, client_addr_len);
                }
                
                //data in socket
                else{
                    //reading from the socked into buffer
                    message_size = recvfrom(sockfd, buff, buff_len, 0, (struct sockaddr*)client_addr, &client_addr_len);

                    //fail in reading from socket
                    if(message_size < 0){
                        sys_call_error("TTFTP_ERROR: revcfrom faild");
                    }

                    else{
                        //fix bit representation
                        buff->opcode = ntohs(buff->opcode);
                        buff->block_number = ntohs(buff->block_number);
                        break;
                    }
                }
            }

            //worng block number - fatal error
            if(buff->block_number != block_number+1){
                send_error(sockfd, INVALID_BLOCK_NUM, "Bad block number", client_addr, client_addr_len);
                return;
            }

            //unexpected packet - fatal error
            if(buff->opcode != DATA_OPCODE){
                send_error(sockfd, INVALID_BLOCK_NUM, "Unexpected packet", client_addr, client_addr_len);
                return;
            }

        } while (false);// remove this condition?*************************************

        //recieved packet is valid
        missed_packets = 0;

        //writing the recived data into the file
        if (fwrite(buff->data, 1, message_size - (PACKET_SIZE+DATA_SIZE), file_ptr) != message_size - (PACKET_SIZE+DATA_SIZE)){
            sys_call_error("TTFTP_ERROR: fwrite faild");
        }

        //sending ack to client
        block_number++;
        send_ack(sockfd, block_number, client_addr, client_addr_len);
    }while(message_size >= PACKET_SIZE);
}                        
                         
FILE* wrq_parser(int sockfd, struct sockaddr_in* client_addr, socklen_t client_addr_len, WRQ* wrq){
    wrq->opcode= ntohs(wrq->opcode);
    if(wrq->opcode != WRQ_OPCODE ){
        send_error(sockfd, DATA_BEFORE_WRQ,"Unknown user", client_addr, client_addr_len);
        return NULL;
    }

    if(strcmp(wrq->trans_mode, "octet") != SUCCESS){
       send_error(sockfd, DATA_BEFORE_WRQ,"Wrong Transmission mode ", client_addr, client_addr_len);
       return NULL;
    }

    // open thr requested file with wrtie premisson
    FILE* file_ptr = fopen((const char*)wrq->file_name, "w");
    if(file_ptr == NULL){
        send_error(sockfd, FILE_ALREADAY_EXSITS, "File already exists", client_addr, client_addr_len);
        return NULL;
    }

    return file_ptr;
}
