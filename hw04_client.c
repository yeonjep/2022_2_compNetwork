//2021115184 박연제
//TCP(SEQ,ACK)이용한 파일 전송
//client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 100
#define SEQ_START 1000

typedef struct{
        int seq;              //SEQ number
        int ack;               //ACK number
        int buf_len;          //File read/write bytes
        char buf[BUF_SIZE];   //파일이름 또는 파일 내용전송
}Packet;
void error_handling(char* message);

int main(int argc, char* argv[])
{
    char fname[BUF_SIZE];
    char message[] = "File Not Found";
    int str_len = 0;
    int fdout = 0;
    int len = 0;

    //-------------------------------------------------------------------tcp set
    int sock;
    struct sockaddr_in serv_addr;
    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");

    //-------------------------------------------------------------------------

    Packet p; //구조체 생성

    printf("Input file name: "); //filename
    scanf("%s", p.buf);//클라이언트는 사용자로부터 전송받을 파일의 이름을 입력받음
// 여기서 패킷 구조체 멤버인 버퍼로 이름을 받아와야 함 -> 수정하기:

    printf("[Client] request :%s\n", p.buf);
    printf("\n");

    //1) client ->  server 파일명 전송하기
    write(sock, &p, sizeof(p));

    strcpy(fname,p.buf);

    char buf_read[BUF_SIZE];
 
    //2) server -> client : File Found or Not Found(파일이 서버에 없는 경우)
    str_len = read(sock, buf_read, sizeof(buf_read));
    if (strcmp(buf_read, message) == 0) { //File Not Found랑 일치하면 출력 및 소켓종료
        printf("%s\n", message);
        return 0;
    }

    fdout = open(p.buf, O_CREAT | O_WRONLY | O_TRUNC, 0644); //파일 생성
    if (fdout == -1) {
        error_handling("open() error");//open() 에러
    }

    while (1)
    {
        //3) server -> clien 
        str_len = read(sock, &p, sizeof(p));
        /* if (str_len == -1)
            error_handling("read() error");*/

        /* if (p.buf_len == 0)
            break;//탈출 */

        p.ack = p.seq + p.buf_len + 1;
        printf("[Client] Rx SEQ : %d, len: %d bytes\n", p.seq, p.buf_len);

        if (p.buf_len != BUF_SIZE) //마지막 데이터 
            break;
        //4) client -> server : packet(ack)
        write(sock, &p, sizeof(p));
        printf("[client] Tx ACK: %d\n", p.ack); //서버에 보낸 ack 출력
   printf("\n");

        int size = write(fdout, p.buf, p.buf_len);
        if (size == -1)
            error_handling("write() error!");
        len += p.buf_len;
    }
    
    write(fdout, p.buf, p.buf_len);
    len += p.buf_len; //add last data
    printf("%s received (%d Bytes)\n\n", fname, len);

    close(sock);

    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
