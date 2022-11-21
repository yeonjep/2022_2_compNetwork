//2021115184 박연제
// packet forwarding client 및 server 구현_ client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

#define BUF_SIZE 2048

void error_handling(char *buf);

int main(int argc, char *argv[])
{
    int fd1; //file open (파일읽기 용도)
    int fd2; //server sock (서버 통신용도)
    struct timeval timeout;
    fd_set reads, cpy_reads;
    int fd_max, fd_num, choice;
    char message[BUF_SIZE];

    FD_ZERO(&reads);           //FD_ZERO 사용: 모두 0으로 초기화
   
    //----------------------------TCP set-------------------------//
    int str_len;
    struct sockaddr_in serv_adr;

    if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
  
        printf("---------------------------------\n");
        printf("Choose function\n");
        printf("1. Sender,   2. Receiver\n");
        printf("---------------------------------\n");
        printf("=> ");
        scanf("%d",&choice);
        
	if(choice==1){
	printf("File Sender Start!\n");}
	else if(choice==2){
	printf("File Receiver Start!\n");}
	else{
	printf("error!");}

	// fd1 set
   	 if(choice==1){
   		 fd1 = open("rfc1180.txt", O_RDONLY); //fd1 :rfc1180.txt 파일 열기
   		 if (fd1 == -1)
    		{
       			 error_handling("open() error");
    		}
   		 FD_SET(fd1,&reads); //FD_SET 사용: fd1,fd2를 관찰대상으로 추가
	}


	//fd2 set
	fd2=socket(PF_INET, SOCK_STREAM, 0); //fd2 : server sock 
	if(fd2==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));

       
	if(connect(fd2, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");

       //----------------------------------------------------------//
        char str1[10]="Sender";
	char str2[10]="Receiver"; 

        if(choice==1)  
		write(fd2, str1, strlen(str1));// write()
	if(choice==2)
                write(fd2, str2, strlen(str2));// write()


        FD_SET(fd2, &reads);   //FD_SET 사용: fd1,fd2를 관찰대상으로 추가
        fd_max = fd2;

	if(choice==1)   //sender print
		printf("fd1: %d, fd2: %d\n", fd1, fd2);
	if(choice==2)   //receiver print
		printf("fd2: %d\n", fd2);

	printf("fd_max: %d\n", fd_max);
       //------------------------------------------------------------

	while(1) 
	{
        cpy_reads = reads;
        timeout.tv_sec = 3;        //timeout 3초
        timeout.tv_usec = 0;

        if((fd_num=select(fd_max+1, &cpy_reads, 0,0,&timeout))==-1) //select error
            break;
        if(fd_num==0) //timeout 
            continue;

	switch(choice){
 	
	case 1://sender
               if(FD_ISSET(fd2,&cpy_reads))
               {
                    message[str_len] = 0;
	            str_len=read(fd2,message,BUF_SIZE);
                    write(1,message,strlen(message)); //write
                  
               }
               if(FD_ISSET(fd1,&cpy_reads)) //FD_ISSET 사용(변환된 정보가 있다면 if 문 수행)
               {  
		     sleep(1);
                     str_len = read(fd1,message,BUF_SIZE);
                     if(str_len == 0)         // 읽어온 내용이 없으면
                     {
                         FD_CLR(fd1,&reads);  //FD_CLR 사용(제외 후 종료)
                         close(fd1);
                     }
                     write(fd2,message,strlen(message));
               }
        
	break;
	
	case 2://receiver
	
       		 if(FD_ISSET(fd2,&cpy_reads))
       		 {
           		 str_len = read(fd2,message,BUF_SIZE); //read
           		 message[str_len] = 0; 
                         write(1, message, strlen(message)); //write
		         write(fd2, message, strlen(message)); 
                 }
        
	break;

       }
}
        FD_CLR(fd2,&reads);
        close(fd2);
	return 0;

}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
