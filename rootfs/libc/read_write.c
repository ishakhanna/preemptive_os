#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"

ssize_t write(int fildes, const void *buf, size_t nbytes){
	ssize_t ret;
    ret = (ssize_t)syscall_3(SYS_write, (uint64_t)fildes,(uint64_t)buf,(uint64_t)nbytes);
	if(ret < 0){
		errno = (-1)*ret;
		return -1;
	}
	else
		return ret;
}

ssize_t read(int fildes, void *buf, size_t nbytes){
	int ret = (int) syscall_3(SYS_read, (uint64_t) fildes, (uint64_t) buf, (uint64_t) nbytes);

	if(ret < 0){
	 	errno = (-1)*ret;
		return -1;
	}
	return ret;
}
