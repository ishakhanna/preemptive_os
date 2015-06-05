#include <../include/stdio.h>
#include <../include/stdlib.h>
#include <../include/syscall.h>
int execve(const char *filename, char *const argv[], char *const envp[])
{
	//int ret;
	//ret = syscall_3(SYS_execve, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
	syscall_3(SYS_execve, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
/*	__asm__ __volatile__(
        "syscall"
        : "=a" (ret)
        : "a"(SYS_execve), "D"((uint64_t)filename), "S"((uint64_t)argv), "d"((uint64_t)envp)
);*/
	
	errno=2;
	return -1;
}
