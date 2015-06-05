#include "sys/syscall.h"
#include "syscall.h"
#include "stdlib.h"
#include "stdio.h"

int kill(int pid)
{
        int result = 0;

        result = (int) syscall_1(SYS_kill, (uint64_t)pid);
        if(result < 0)
        {
                errno = (-1) * result;
                return -1;
        }
        return result;
}

int ps()
{
        int result = 0;

        result = (int) syscall_0(SYS_ps);
        if(result < 0)
        {
                errno = (-1) * result;
                return -1;
        }
        return result;
}
