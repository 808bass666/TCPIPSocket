#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>     
#include <unistd.h>
#include <arpa/inet.h>  
#include <sys/socket.h> 
#define BUF_SIZE 30 
void error_handling(char* message);

int main(int argc, char* argv[]) {
    int  serv_sock, clnt_sock;
    FILE *fp;
    struct sockaddr_in serv_adr, clnt_adr; 
    socklen_t clnt_adr_sz; 
    char buf[BUF_SIZE];
    
    if(argc != 2) { 
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   

	if((fp = fopen("halfclose_serv.c", "rb"))== NULL) printf("File Open Failed !\n");
    else printf("File Open Success !\n");
    
	serv_sock = socket(PF_INET, SOCK_STREAM, 0); 
    if(serv_sock == -1) error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr)); 
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1])); 
    
    int on = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)  error_handling("setsockopt() error");
    
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error"); 
    
    if(listen(serv_sock, 5) == -1) error_handling("listen() error");
    
    clnt_adr_sz = sizeof(clnt_adr); 
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz); 
    if(clnt_sock == -1) error_handling("accept() error");
    
    // Read data from the file and write the file to client
    int read_cnt = 0;
	while (1) {
		read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
        //从fp文件描述符中读数据到buf
        if (read_cnt < BUF_SIZE)  {
        	write(clnt_sock, buf, read_cnt); //将从文件中读到的read_cnt个字节写入客户端
        	break; //写完就退出
        }
        //如果read_cnt >= BUF_SIZE，也只能先将BUF_SIZE大小的数据写到客户端，之后继续循环传剩下的
        write(clnt_sock, buf, BUF_SIZE); 
	}
    shutdown(clnt_sock, SHUT_WR);//关闭写的那一半
	read(clnt_sock, buf, BUF_SIZE);//继续从客户端读取数据
	printf("Message from client: %s \n", buf);

	fclose(fp);
    close(clnt_sock); 
    close(serv_sock); 
    return 0;
}
void error_handling(char* message) {
    perror(message);  
    exit(1);  
}
