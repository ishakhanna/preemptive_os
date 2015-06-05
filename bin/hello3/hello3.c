#include <stdio.h>
#include <stdlib.h>
#include "string_helper.h" 

int main(int argc, char* argv[], char* envp[] ){
int fd = open("libc/printf.c", 0);
int pid = fork();
printf("Back from 1st fork fd : %d\n", fd);
if(pid == 0){
	printf("In child of parent1(hello1)\n");
	close(fd);
	exit(0);
}else{
	printf("In parent(hello1) with child pid %d\n", pid);
	close(fd);
	yield();
	while(1);
	printf("Calling fork again\n");
	int pid2 = fork();
	if(pid2 == 0){
		printf("SHOULDNT COME HERE\n");
	}
	else{
		printf("In parent(hello1) with 2nd child pid %d\n", pid);
//		waitpid(pid2, 0,0);
		printf("Back in parent1(hello1)\n");
	}
}
while(1);
return 0;
}
