
#include <../include/stdio.h>
#include <../include/stdlib.h>
#include <../include/syscall.h>
pid_t waitpid(pid_t pid, int *status, int options)
{
	 printf("Inside my WAIT function\n");
        int ret;
        __asm__ __volatile__(
        "syscall"
        : "=r" (ret)
        : "a"(SYS_wait4), "D"((uint64_t)pid), "S"((uint64_t)status), "d"((uint64_t)options)
);
        printf("Returned from my WAIT  function\n");

        if(ret<0)
        {
		errno= (-1)*ret;
                return -1;
        }	
	return ret;
}
