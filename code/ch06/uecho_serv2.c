#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>     
#include <unistd.h>    
#include <arpa/inet.h>
#include <sys/socket.h> 
#define BUF_SIZE 30
void error_handling(char* message); 

int main(int argc, char* argv[]) {
    int  serv_sock;
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; 
    
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   
    
    // Creates a UDP socket.
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0); 
    if(serv_sock == -1) error_handling("UDP socket() creation error");
   
    //Initialize the server address structure 
    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
   
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
     
    int str_len;
    clnt_adr_sz = sizeof(clnt_adr); //initialize clnt_adr_sz
    while (1) {
        //不限制数据传输对象，因此第一个参数不是clnt_sock而是serv_sock
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        message[str_len] = 0;
        printf("Message from clnt: %s", message);
        
        fputs("Insert message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) break;

        sendto(serv_sock, message, strlen(message), 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
    }
    close(serv_sock); 
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1);  
}
