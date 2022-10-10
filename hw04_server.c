//2021115184 박연제
//TCP(SEQ,ACK)파일 전송
//hw4_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 100
#define SEQ_START 1000
typedef struct {
    int seq;
    int ack;
    int buf_len;
    char buf[BUF_SIZE];
}Packet;

void error_handling(char* message);

int main(int argc, char* argv[])
{

    char fname[BUF_SIZE];//filename
    char message[] = "File Not Found";
    int str_len = 0;
    int fd = 0;
    int len = 0;

    /////////////////////////////////tcp////////////////////////////////
    int serv_sock;
    int clnt_sock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    printf("----------------------------------\n");
    printf(" File Transmission Server \n");
    printf("----------------------------------\n");

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error");
    ////////////////////////////////tcp///////////////////////////////////////
    

    Packet p; //구조체 생성

    //1) client -> server (파일이름)
    str_len = read(clnt_sock, &p, sizeof(p));
    if (str_len == -1)
        error_handling("read() error");
    strcpy(fname,p.buf);

    fd = open(fname, O_RDONLY); //파일 열기
    if (fd == -1) { //-1 : 실패, 읽어들인 바이트 수 : 성공
        printf("%s File Not Found\n", fname);
        write(clnt_sock, message, sizeof(message)); //2) server -> client : File Not Found
        return 0;
    }

    //2))파일 찾음
    //server ->client : File Found
    write(clnt_sock, fname, sizeof(fname));
    printf("[server] sending %s\n", fname);
    printf("\n");

    p.ack = SEQ_START;
    while (1)
    {
        p.seq = p.ack;
        p.buf_len = read(fd, p.buf, BUF_SIZE); //test.txt 파일, buf에다가 저장하면서 100바이트 씩 읽어와
        len += p.buf_len; //읽어오는 바이트 수 누적

        if (p.buf_len == -1)
            error_handling("read error()");
        /* if (p.buf_len == 0) //stop //더 이상 읽어들일 게 없음
            break; */

        printf("[Server] Tx: SEQ: %d, %d byte data\n", p.seq, p.buf_len);

        //3)server -> client : packet send
        write(clnt_sock, &p, sizeof(p));

        if (p.buf_len != BUF_SIZE) //읽어오는 바이트 수 < 100바이트 stop 
            break; //탈출

         //4) client ->server : packet(ack)
        str_len = read(clnt_sock, &p, sizeof(p));
        if (str_len == -1)
            error_handling("read() error");
        printf("[Server] Rx ACK : %d\n\n", p.ack);

    }

    printf("%s sent (%d Bytes)\n", fname, len);
    close(clnt_sock);
    close(serv_sock);

    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
