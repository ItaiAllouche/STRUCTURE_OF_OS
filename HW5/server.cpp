#include "server.h"

void send_error(int sockfd, u_short error_code, string error_message, SOCK_ADDR* client_addr, socklen_t client_addr_len){

    //initiliztion of error packet
    ERROR error = {0};
    error.opcode = htons(ERROR_OPCODE);
    error.error_code = htons(error_code);
    error.error_message = error_message;

    if(sendto(sockfd, &error, sizeof(error), 0, (struct sockaddr*) client_addr, client_addr_len) < 0){
        sys_call_error("TTFTP_ERROR: sendto faild");
    }
    return;
}

void recieving_data_mode(int sockfd, BUFFER* buff, size_t buff_len, FILE* file_ptr, short_int timeout,
                         SOCK_ADDR* client_addr, socklen_t client_addr_len, short_int max_num_of_resends){
                            
    fd_set readfds;
    struct timeval corrent_time;  // as recommended on pdf
    int data_to_read = 0;
    u_short block_number;
    int missed_packets = 0;
    int message_size = 0;

    //FD_ZERO(&fd_recieve); //  Initializes the file descriptor set fdset to have zero bits for all file descriptors.
    //FD_SET(sockfd, &fd_recieve);  //  Sets the bit for the file descriptor fd in the file descriptor set fdset.

    while(message_size >= PACKET_SIZE){
            //initilize buffer
            memset(buff, 0, buff_len); 

            //validate the revieved data 
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

                    // no data was in the socket in the period of time
                    else if(data_to_read == 0){
                        missed_packets++;
                        send_ack(sockfd, block_number, client_addr, client_addr_len);
                    }
                    
                    //socket has data
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

                    // too many packets were missed - fatal error
                    if(missed_packets >= max_num_of_resends){
                        send_error(sockfd, REACHED_MAX_RESENDS, "Abandoning file transmission", client_addr, client_addr_len);
                        return;
                    }
                }

                //the packet is not data - fatal error
                // check what happens when receiving wrong opcode
                if(buff->opcode != DATA_OPCODE || buff->block_number != block_number+1){
                    send_error(sockfd, INVALID_BLOCK_NUM, "Bad block number", client_addr, client_addr_len);
                    return;
                }
            } while (false);// remove this condition?

            //recieved packet is valid
            missed_packets = 0;

            //writing the recived data into the file
            if (fwrite(buff->data, 1, message_size - (PACKET_SIZE+DATA_SIZE), file_ptr) != message_size - (PACKET_SIZE+DATA_SIZE)){
                sys_call_error("TTFTP_ERROR: fwrite faild")
            }

            //sending ack to client
            block_number++;
            send_ack(sockfd, block_number, client_addr, client_addr_len);
    }
                         }
                         

