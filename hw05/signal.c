#include<stdio.h>
#include<unistd.h>
#include<signal.h>

void timeout(int sig)
{
	if(sig==SIGALRM)
		puts("time out!");
	alarm(2);
}
void keycontrol(int sig)
{
	if(sig==SIGINT)
		puts("CTRL+C pressed");
}
int main(int argc, char *argv[])
{
	int i;
	signal(SIGALRM,timeout);
	signal(SIGINT,keycontrol);

	alarm(2);
	for(i=0;i<3;i++){
		printf("wait: %d\n",i);
		sleep(100);

}
printf("i=%d\n",i);
return 0;
}
