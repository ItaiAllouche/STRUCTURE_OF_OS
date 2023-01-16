#include "tftp.h"

int main(int argc, char *argv[]){

    //arg validation
    if(argc != NUM_OF_ARGS || !atoi(argv[1]) || !atoi(argv[2]) || !atoi(argv[3])){
        arg_error();
    }

    u_short port_num = atoi(argv[1]);
    u_short timeout  = atoi(argv[2]);
    u_short max_num_of_resends  = atoi(argv[3]);
    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = (INADDR_ANY);
    server_addr.sin_port = htons(port_num);
    char wrq_buff [PACKET_SIZE];
    WRQ wrq_strcut = {0};
    BUFFER data_buff = {0};
    FILE* file_ptr = NULL;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        sys_call_error("TTFTP_ERROR: socket failed", sockfd);
    }
 
    if(bind(sockfd, (struct sockaddr *) & server_addr, sizeof(server_addr)) < 0 ){
        sys_call_error("TTFTP_ERROR: bind failed", sockfd);   
    }


    while(true){
        //initilize buffers
        memset(&wrq_buff, 0, 5);
        memset(&data_buff, 0, sizeof(data_buff));

        // expecting to receive wrq 
        int received_msg_size = recvfrom(sockfd, wrq_buff, sizeof(wrq_buff), 0, (struct sockaddr*) & client_addr, &client_addr_len);
        if(received_msg_size < 0){
            sys_call_error("TTFTP_ERROR: recvfrom failed", sockfd);
        }

        wrq_parser(&wrq_strcut, wrq_buff);
        if(wrq_strcut.opcode != WRQ_OPCODE){
            send_error(sockfd, DATA_BEFORE_WRQ, "Unknown user", &client_addr, client_addr_len);
        }

        else if(file_is_exist(wrq_strcut.file_name)){
            send_error(sockfd, FILE_ALREADAY_EXSITS,"File already exists", &client_addr, client_addr_len);
        }

        else{
            file_ptr = fopen(wrq_strcut.file_name, "w");
            if(file_ptr == NULL){
                sys_call_error("fopen faild", sockfd);
            }
            
            send_ack(sockfd, 0, &client_addr, client_addr_len);
            recieving_data_mode(sockfd, &data_buff, sizeof(data_buff), file_ptr, wrq_strcut.file_name, timeout, &client_addr, client_addr_len, max_num_of_resends);
            
            if(fclose(file_ptr) != SUCCESS){
                sys_call_error("TTFTP_ERROR: fclose failed", sockfd);
            }
        }
    }
    close(sockfd);
    return 0;
}