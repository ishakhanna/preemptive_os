#include <../include/stdlib.h>
#include <../include/syscall.h>
pid_t fork(void)
{
	pid_t ret;
	ret = (pid_t)syscall_0(SYS_fork);
	if(ret<0)
	{
		errno = (-1) * ret;
                return -1;
	}
	return ret;
}

pid_t getpid(void)
{
	pid_t ret;
	ret = (pid_t)syscall_0(SYS_getpid);
        return ret;
}

pid_t getppid(void)
{
        pid_t ret;
        ret = (pid_t)syscall_0(SYS_getppid);
        return ret;
}

unsigned int sleep(unsigned int seconds)
{
         unsigned int ret;
        uint64_t input_ns=seconds;
        ret = syscall_1(SYS_nanosleep, input_ns);
/*        __asm__ __volatile__(
        "syscall"
        : "=a" (ret)
        : "a"(SYS_nanosleep), "D"(input_ns)
);*/
        return ret;
}

unsigned int alarm(unsigned int seconds)
{
	unsigned int ret;	
  	ret = syscall_1(SYS_alarm, seconds);
/*	__asm__ __volatile__(
        "syscall"
        : "=r" (ret)
        : "a"(SYS_alarm), "D"((uint64_t)seconds)
);*/
	return ret;
}
