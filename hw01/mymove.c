#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 100
//2021115184 박연제
//저수준 파일 입출력
//open(),read(),write() 사용

void error_handling(char* message);
int main(int argc, char* argv[])
{
   int fd1, fd2;
   char buf[BUF_SIZE]; 
   int size_r,size_w;
   int cnt=0;


   if (argc!=3)
       error_handling("[Error] mymove Usage: ./mymove src_file dest_file");


   fd1=open(argv[1], O_RDONLY);
        printf("%s",argv[1]);

   if(fd1==-1)
      error_handling("open*`() error!");

  // size_r = read(fd1, buf, sizeof(buf));                  

   fd2=open(argv[2],O_CREAT|O_WRONLY|O_TRUNC,0644);
   if(fd2==-1)
      error_handling("open() error!");

   size_w = write(fd2, buf,size_r);

   while(size_r>0){
            size_r = read(fd1, buf, sizeof(buf));
           if(size_w!=size_r)
                   error_handling("write() error!");
           else
                   cnt+=size_r;
   }

   printf("move from %s to %s (bytes: %d) finished\n",argv[1],argv[2],cnt);


   close(fd1);
   close(fd2);
   remove(argv[1]);

   return 0;
}

void error_handling(char* message)
{
   fputs(message,stderr);
   fputc('\n', stderr);
   exit(1);
}
         
