//2021115184 박연제
//hw2_client.c
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include<arpa/inet.h>

#define BUF_SIZE 1024
void error_handling(char *message);

typedef struct{

        int cmd;                //0: request, 1: response, 2: quit
        char addr[20];          //dotted-decimal address 저장(20bytes)
        struct in_addr iaddr;   //inet_aton() result 저장
        int result;             // 0: Error, 1: Success

}PACKET;
int main(int argc, char* argv[],PACKET p1,PACKET p2){

        int sock;
        struct sockaddr_in serv_addr;
        int r_len;
        char send_msg[BUF_SIZE];
        char rcv_msg[BUF_SIZE];

        if(argc!=3){
                printf("Usage : %s <IP> <port>\n", argv[0]); // 
                exit(1);// ip주소나 포트번호 미입력시 error 처리
        }
        sock=socket(PF_INET, SOCK_STREAM, 0);
        if(sock == -1)
                error_handling("socket() error");
          
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family=AF_INET;
        serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
        serv_addr.sin_port=htons(atoi(argv[2]));
      
        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
                error_handling("connect() error!");
    
        while (1)
        {
		p1.cmd=0;p1.result=0;
                p2.cmd=0;p2.result=0;

                printf("Input dotted-decimal address: ");
                scanf("%s",send_msg);
    
                strcpy(p1.addr,send_msg);

                write(sock, &p1, sizeof(p1));//서버로 전송
	        if (strcmp(p1.addr, "quit") == 0)
                {
                        p2.cmd=2;
                        printf("[Tx] cmd: %d(QUIT)\n",p2.cmd);
                        printf("Client socket close and exit\n");
                        break;
                }

                r_len = read(sock,&p2, sizeof(p2));//서버에서 패킷 읽어옴
                if (r_len == -1)
                        error_handling("read() error!");
               
               printf("[Tx] cmd: %d, addr: %s\n",p1.cmd,p1.addr);
               
               if(p2.result==0)//fail
                        printf("[Rx] cmd: %d, Address conversion fail! (result: %d)\n\n",p2.cmd,p2.result);

               if(p2.result==1){
		       
                       printf("[Rx] cmd: %d, Address conversion: %#x (result: %d)\n\n",p2.cmd,p2.iaddr.s_addr,p2.result);

        }}


        close(sock);

        return 0;
}
void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}
