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

    if(sendto(sockfd, (char*)&ack, sizeof(ack), 0, (struct sockaddr*) client_addr, client_addr_len) != sizeof(ack)){
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
    struct sockaddr_in curr_client_addr = {0};
    socklen_t curr_client_addr_len = sizeof(curr_client_addr);

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
                    message_size = recvfrom(sockfd, (char*)buff, buff_len, 0, (struct sockaddr*)&curr_client_addr, &curr_client_addr_len);

                    //message recevied from wrong client
                    if(client_addr->sin_addr.s_addr != curr_client_addr.sin_addr.s_addr){
                        send_error(sockfd, DATA_BEFORE_WRQ, "Unknown user", &curr_client_addr, curr_client_addr_len);
                    }

                    //fail in reading from socket
                    else if(message_size < 0){
                        sys_call_error("TTFTP_ERROR: revcfrom faild");
                    }

                    else{
                        //fix bit representation
                        buff->opcode = ntohs(buff->opcode);
                        buff->block_number = ntohs(buff->block_number);
                        break;
                    }
                }
            }//while true

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
        if (fwrite(buff->data, sizeof(char), message_size - (PACKET_SIZE-DATA_SIZE), file_ptr) != message_size - (PACKET_SIZE-DATA_SIZE)){
            sys_call_error("TTFTP_ERROR: fwrite faild");
        }

        //sending ack to client
        block_number++;
        send_ack(sockfd, block_number, client_addr, client_addr_len);
    }while(message_size >= PACKET_SIZE);
}                        
                         
void wrq_parser(WRQ* wrq_struct, char wrq_buff [PACKET_SIZE]){
    memcpy(&wrq_struct->opcode, wrq_buff, 2);
    wrq_struct->opcode= ntohs(wrq_struct->opcode);
    strncpy(wrq_struct->file_name, wrq_buff + 2,strlen(wrq_buff + 2));
    return;
}

bool file_is_exist(const char *fileName){
    std::ifstream infile(fileName);
    return infile.good();
}