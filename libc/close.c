#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"


int close(int fd)
{
        int result = 0;

        if(fd < 0)
        {
                errno = 9; //MYEBADF;
                return -1;
        }

        result = (int) syscall_1(SYS_close, (uint64_t) fd);

        if(result != 0)
        {
                errno = (-1) * result;
                return -1;
        }

        return result;
}

