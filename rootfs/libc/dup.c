#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"

int dup(int old_fildes){
	int ret = (int) syscall_1(SYS_dup, (uint64_t)old_fildes);
	if(ret < 0){
		errno = (-1)*ret;
		return -1;
	}
	return ret;
}
int dup2(int oldfd, int newfd){
	int ret = (int) syscall_2(SYS_dup2, (uint64_t) oldfd, (uint64_t) newfd);
        if(ret < 0){
                errno = (-1)*ret;
                return -1;
        }
        return ret;
}	
