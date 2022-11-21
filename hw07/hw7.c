//2021115184 박연제
//멀티캐스트를 이용한 채팅 프로그램(sender와 receiver를 하나의 소스로 구현)
// -> fork() 호출을 통한 멀티 프로세서 생성후 조건문으로 child/parent process로 나누어 구현
//구조체 -> 입력 시 name / message store
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define TTL 64
#define BUF_SIZE 120

typedef struct{
    char name[20];
    char message[BUF_SIZE];
}PERSON;

void error_handling(char *message);
int main(int argc, char *argv[])
{
    int send_sock, recv_sock;
    struct sockaddr_in adr;      // receiver
    struct ip_mreq join_adr;     // receiver
    struct sockaddr_in mul_adr;  // sender

    int time_live = TTL;         // TTL
    char buf[BUF_SIZE];
//    char name[10];//?

    PERSON person;//?
    int option =1; 
    pid_t pid;
    

    //9190까지만 입력 시
    if(argc != 4) {
        printf("Usage : %s <GroupIP> <PORT> <Name>\n", argv[0]);
	exit(1);
    }

    //input person's name(ex.Alice, Bob, Charlie) -> person.name
    strcpy(person.name, argv[3]);

    //---------------------sender socket-------------------------------
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&mul_adr, 0, sizeof(mul_adr));

    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]);  //Multicast IP(224.1.1.2)
    mul_adr.sin_port=htons(atoi(argv[2]));         //Multicast Port
    //-----------------------------------------------------------------------


    //multicast TTL 설정
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));

    //----------------------receiver socket-------------------------------
    recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));

    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[2]));
    //--------------------------------------------------------------------

    //멀티캐스트 그룹 가입
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_adr, sizeof(join_adr));


    //SO_REUSEADDR 
    setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));//3개-> bind() error handle

    if (bind(recv_sock, (struct sockaddr *)&mul_adr, sizeof(mul_adr)) < 0)
    {
        error_handling("bind error()");
    }

    pid = fork();// multi process 생성

    if (pid < 0)
    {
        error_handling("fork() error");
    }

    else if (pid == 0) //child process (receive)
    {
        int str_len;
        while(1)
        {
            str_len = recvfrom(recv_sock, &person, sizeof(person), 0, NULL, 0); // 데이터 수신
             // if(str_len < 0)
             //   break; 
            person.message[str_len] = 0; 
            printf("received Message: [%s] %s",person.name, person.message);
            
        };
    }
    else //parent process : input + send 
    { 
      
        while (1)
        {
            fgets(person.message,BUF_SIZE, stdin);
            if(strcmp(person.message,"q\n")==0 || strcmp(person.message,"Q\n")==0)// 종료조건
             {
                 puts("Leave multicast group");
                 break;
             }
            sendto(send_sock, &person, sizeof(person), 0,(struct sockaddr *)&mul_adr, sizeof(mul_adr));//데이터 전송
        };
    }
    close(send_sock);
    close(recv_sock);
    
    return 0;


}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
