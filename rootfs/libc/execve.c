#include <../include/stdio.h>
#include <../include/stdlib.h>
#include <../include/syscall.h>
int execve(const char *filename, char *const argv[], char *const envp[])
{
	printf("Inside my EXECVE function\n");
	int ret;
	__asm__ __volatile__(
        "syscall"
        : "=r" (ret)
        : "a"(SYS_execve), "D"((uint64_t)filename), "S"((uint64_t)argv), "d"((uint64_t)envp)
);
	printf("Returned from my EXECVE function\n");

	if(ret<0)
	{
		printf("Error in executing");
		errno=(-1)*ret;
		return -1;
	}
	exit(0);
	return 0;

}
