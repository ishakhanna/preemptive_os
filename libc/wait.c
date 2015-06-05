
#include <../include/stdio.h>
#include <../include/stdlib.h>
#include <../include/syscall.h>
pid_t waitpid(pid_t pid, int *status, int options)
{
        int ret;
/*        __asm__ __volatile__(
        "syscall"
        : "=r" (ret)
        : "a"(SYS_wait4), "D"((uint64_t)pid), "S"((uint64_t)status), "d"((uint64_t)options)
);*/

	ret = syscall_3(SYS_wait4, (uint64_t) pid, (uint64_t)status, (uint64_t)options);
        if(ret<0)
        {
		errno= (-1)*ret;
                return -1;
        }	
	return ret;
}
