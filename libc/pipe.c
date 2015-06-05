#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"

int pipe(int fildes[]){
	int ret = (int) syscall_1(SYS_pipe, (uint64_t)fildes);
	if(ret < 0){
		errno = (-1)*ret;
		return -1;
	}
	return ret;
}
