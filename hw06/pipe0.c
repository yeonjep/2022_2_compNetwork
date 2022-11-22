#include<stdio.h>
#include<unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
	int fds[2];
	char str[]="Self pipe test";
char buf[BUF_SIZE];
pid_t pid;

pipe(fds);
printf("fds[0]=%d, fds[1]=%d\n",fds[0],fds[1]);

write(fds[1],str,sizeof(str));

read(fds[0],buf,BUF_SIZE);
puts(buf);
return 0;
}
