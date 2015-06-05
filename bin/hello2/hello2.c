#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[] ){
	printf("\nIN Hello2\n");
	while(1);
	int pid = fork();
//	printf("\n-----Fork returned : %d------\n", pid);
	if(pid == 0){
		printf("In child Hello2\n");
//		while(1);
		char *args[] = {"bin/ps", NULL};
		char *en[] = {"PATH=/rootfs/bin", "CWD=/rootfs", NULL};
		execve("bin/ps", args, en);

	}else{
		waitpid(pid, 0, 0);
		printf("Back in hello2 parent pid: %d\n", getpid());
	}
while(1);
return 0;
}
