#include "tftp.h"

int main(int argc, char *argv[]){

    //arg validation
    if(argc != NUM_OF_ARGS || !atoi(argv[1]) || !atoi(argv[2]) || !atoi(argv[3])){
        //cout << argc << " " << atoi(argv[1]) << " "  <<  atoi(argv[2]) << " "  << atoi(argv[3]) << endl;
        arg_error();
    }

    u_short port_num = atoi(argv[1]);
    u_short timeout  = atoi(argv[2]);
    u_short max_num_of_resends  = atoi(argv[3]);
    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len = sizeof(client_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_num);
    WRQ wrq = {0};
    char aaa [512];
    BUFFER buff = {0};
    int received_msg_size = 0;
    FILE* file_ptr = NULL;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        sys_call_error("TTFTP_ERROR: socket failed");
    }
 
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ){
        sys_call_error("TTFTP_ERROR: bind failed");   
    }


    while(true){

        //initilize buffers
        memset(&aaa, 0, 5);
        memset(&buff, 0, sizeof(buff));

        received_msg_size = recvfrom(sockfd, aaa, 512, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if(received_msg_size < 0){
            sys_call_error("TTFTP_ERROR: recvfrom failed");
        }

        file_ptr = wrq_parser(sockfd, &client_addr, client_addr_len, &wrq);
        if(file_ptr == NULL){
            continue;
        }

        else{
            send_ack(sockfd, 0, &client_addr, client_addr_len);
            recieving_data_mode(sockfd, &buff, sizeof(buff), file_ptr, timeout, &client_addr, client_addr_len, max_num_of_resends);
            
            if(fclose(file_ptr) != SUCCESS){
                sys_call_error("TTFTP_ERROR: fclose failed");
            }
        }
    }
    return 0;
}