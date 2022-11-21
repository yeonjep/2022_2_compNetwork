//2021115184 박연제
//hw05 fork() 및 sigation()함수를 활용한 다중 타이머 기능 구현
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

//timeout 함수(parent, 2초 간격)
void timeout_p(int sig){
   int static elapsed_time = 0;
   elapsed_time += 2;
   if(sig==SIGALRM)
      printf("<Parent> time out: 2, elapsed time: %d seconds\n", elapsed_time);
   alarm(2);
}
//timeout 함수(child, 5초 간격)
void timeout_c(int sig){
   int static elapsed_time = 0;
   elapsed_time += 5;
   if(sig==SIGALRM)
      printf("[Child] time out: 5, elapsed time: %d seconds\n", elapsed_time);
   alarm(5);
}
//waitpid 함수
void read_childsent(int sig)
{
	int status;
	pid_t id = waitpid(-1, &status, WNOHANG);
        if(WIFEXITED(status))
        	printf("Child id: %d, sent: %d\n", id, WEXITSTATUS(status));
}
int main(void)
{
	pid_t pid;
	//signal handler 등록
        //timeout
        struct sigaction timeoutAct;
        sigemptyset(&timeoutAct.sa_mask);
        timeoutAct.sa_flags=0;

	// waitpid
	struct sigaction waitpidAct;	
	waitpidAct.sa_handler = read_childsent;
	sigemptyset(&waitpidAct.sa_mask);
	waitpidAct.sa_flags = 0;
	sigaction(SIGCHLD, &waitpidAct, 0);

	printf("\n");


	//fork 호출
        pid = fork();

	if(pid==0){//child process -> 20회 반복 후 종료(리턴값:5)

		//handler
		timeoutAct.sa_handler=timeout_c;
		sigaction(SIGALRM,&timeoutAct,0);
		alarm(5);
		for(int i=0;i<20;i++){
			sleep(1);
		}
		return 5;
	}
	else{//parent process -> 무한반복

		//handler
		timeoutAct.sa_handler=timeout_p;
                sigaction(SIGALRM,&timeoutAct,0);
		alarm(2);
		while(1){
			sleep(1);
		}
	}

return 0;
}
