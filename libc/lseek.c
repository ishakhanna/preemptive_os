#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"
#include <stdio.h>

off_t lseek(int fildes, off_t offset, int whence)
{
	if(fildes < 0)
	{
		errno = 9; // EBADF
		printf("Bad file descriptor.\n");
		return (off_t) -1;
	}

	if(whence < 0 || whence > 2)
	{
		errno = 22; // MYEINVAL
		printf("Bad value for whence.\n");
		return (off_t) -1;
	}

	int ret = (int)syscall_3(SYS_lseek, (uint64_t)fildes, (uint64_t)offset, (uint64_t)whence);

	if(ret < 0)
	{
		errno = (-1) * ret;
		return (off_t) -1;	
	}
	return (off_t) ret;
}
