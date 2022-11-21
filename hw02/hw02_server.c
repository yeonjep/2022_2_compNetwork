//2021115184 박연제
//hw2_server.c
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
	struct in_addr iaddr;   //inet_aton() result 저장->ppt 7page
	int result;             // 0: Error, 1: Success

}PACKET;

int main(int argc, char* argv[],PACKET p1,PACKET p2){
	

	int serv_sock;
	int clnt_sock;
	socklen_t clnt_addr_size;
	struct sockaddr_in serv_addr;//소켓의 구조를 담는 기본 구조체 틀의 역
	struct sockaddr_in clnt_addr;

	char send_msg[BUF_SIZE];
	int r_len = 0; 


	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}//명령행 인자의 수가 다른 경우, 에러메시지 출력 후 종료
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//socket 생성
	if(serv_sock == -1) 
		error_handling("socket() error\n");
	
	memset(&serv_addr, 0, sizeof(serv_addr));//0으로 초기화
	serv_addr.sin_family=AF_INET;//주소체계
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//IPv4주소를 저장
	serv_addr.sin_port=htons(atoi(argv[1]));//ppt 7page 참고(포트정보 저장)
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )//bind 실패시 -1 반환
		error_handling("bind() error");

	//클라이언트로의 통신요청 기다림
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	

	clnt_addr_size=sizeof(clnt_addr);  
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);

	if(clnt_sock==-1)
		error_handling("accept() error"); 

	printf("---------------------------\n");
	printf(" Address Conversion Server\n");
	printf("---------------------------\n");

	while (1)
	{
		p1.cmd=0;
		p1.result=0;
		p1.iaddr.s_addr=0;

		p2.cmd=0;
                p2.result=0;
                p2.iaddr.s_addr=0;

		r_len = read(clnt_sock, &p1, sizeof(p1));//from client

		if (r_len == 0)
			break;

		if (strcmp(p1.addr, "quit") == 0) 
		{
			p1.cmd=2;
			printf("[Rx] QUIT message received\n");
			printf("Server socket close and exit.\n");
			break;
		}

		printf("[Rx] Received Dotted-Decimal Address: %s\n", p1.addr);

		if (!inet_aton(p1.addr, &p2.iaddr))
		{
			p2.cmd=1;
			p2.result=0;//error
			printf("[Tx] Address conversion fail: (%s)\n",p1.addr);
		
			write(clnt_sock, &p2, sizeof(p2));
		}
		else//정상인 경우
		{
			p2.cmd=1;
		        p2.result=1;
			printf("inet_aton(%s) -> %#x\n", p1.addr,p2.iaddr.s_addr);
			printf("[Tx] cmd: %d, iaddr: %#x, result: %d\n",p2.cmd,p2.iaddr.s_addr,p2.result);
			write(clnt_sock, &p2, sizeof(p2));
		}
		printf("\n");
	}
	close(clnt_sock);
	close(serv_sock);//데이터 송수신 및 연결해제


	return 0;
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

