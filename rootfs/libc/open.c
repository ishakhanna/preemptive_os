#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"

int open(const char *pathname, int flags)
{
        int result = 0;

        if(pathname == NULL)
        {
                errno = 13; //EACCES
                return -1;
        }

        result = (int) syscall_2(SYS_open, (uint64_t)pathname, (uint64_t)flags);

        if(result < 0)
        {
                errno = (-1) * result;
                return -1;
        }
        return result;

}