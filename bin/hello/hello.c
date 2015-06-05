#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[] ){
printf("\nIN Hello\n");
if(argc > 1){
	printf("\nIN Hello2\n");
	int pid1 = fork();
	if(pid1 == 0){
		printf("In child Hello2\n");
		//char *args[] = {"bin/ps", NULL};
		char *en[] = {"PATH=/rootfs/bin", "CWD=/rootfs", NULL};
		//execve("bin/ps", args, en);
		execve("bin/ps", argv, en);

	}else{
		waitpid(pid1, 0, 0);
		printf("Back in hello2 parent pid: %d\n", getpid());
	}
}
else{
int pid = fork();
if(pid == 0){
	printf("In child Hello\n");
	char *args[] = {"bin/hello", "bin/ps", NULL};
	char *en[] = {"PATH=/rootfs/bin", "CWD=/rootfs", NULL};
	execve("bin/hello", args, en);

}else{
	waitpid(pid, 0, 0);
	printf("Back in hello parent pid: %d\n", getpid());
}
}
//while(1);
return 0;
}
