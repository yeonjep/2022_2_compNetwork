//2021115184 박연제
// packet forwarding client 및 server 구현_ server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 2048
void error_handling(char *buf);

int main(int argc, char *argv[])
{
    int send_sock,receiv_sock, serv_sock, clnt_sock;//-> sender 일때/receiver 일때 로 나눠서 받기
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;

    //-------------------------------------set--------------------------------------------

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    //-------------------------------------------------------------------------------------

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;
    //printf("fd_max= %d\n", fd_max);  

    while(1)
    {
        cpy_reads = reads;
        timeout.tv_sec = 3;  // timeout sec = 3
        timeout.tv_usec = 0; // none

        if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout))== -1)
            break;
        if(fd_num == 0)
            continue;         
        
            if(FD_ISSET(serv_sock, &cpy_reads)) //serv_sock
            {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads);// 클라이언트 소켓을 select() 함수의 감시대상에 추가함.
                    if(fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected client: %d\n",clnt_sock);

                    str_len = read(clnt_sock, buf, BUF_SIZE);// client -> server
           	    buf[str_len] = 0;

	            //문자열 비교 -> send/receive 구분
           	    if (strcmp(buf, "Sender") == 0)
               	        send_sock = clnt_sock;
           	    if (strcmp(buf, "Receiver") == 0)
                        receiv_sock = clnt_sock;

                }
	// backward
        else if (FD_ISSET(receiv_sock, &cpy_reads))//receiv_sock
        {
            str_len = read(receiv_sock, buf, BUF_SIZE);
            if (str_len == 0)//클라이언트가 종료를 했을때(읽을 게 없음)
            {
                FD_CLR(receiv_sock, &reads);
                close(receiv_sock);
                printf("closed client: %d\n", receiv_sock);
            }
            else
            {
                write(send_sock, buf, str_len);//읽었는데 뭔가가 있다면 전송
                printf("Backward [%d] <--- [%d]\n", receiv_sock, send_sock);
            }
        }

        // forward
        else if (FD_ISSET(send_sock, &cpy_reads)) //send_sock
        {
            str_len = read(send_sock, buf, BUF_SIZE);
            if (str_len == 0)////클라이언트가 종료를 했을때(읽을 게 없음)
            {
                FD_CLR(send_sock, &reads);
                close(send_sock);
                printf("closed client: %d\n", send_sock);
            }
            else
            {
                write(receiv_sock, buf, str_len);
                printf("Forward  [%d] ---> [%d]\n", send_sock, receiv_sock);
            }
        }
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}

