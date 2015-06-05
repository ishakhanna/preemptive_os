#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"

void outb(unsigned char value, unsigned short int port){
//	int num = 1;	// no of ports on which you require access
//	int val = 1;   // 1 for getting access, 0 for revoking
//	int ret = (int) syscall_3(SYS_ioperm, (uint64_t) port, (uint64_t) num, (uint64_t) val);

//	if(ret < 0){
//	 	errno = (-1)*ret;
//		return -1;
//	}
	
	int fd = open("/dev/port", O_RDWR);
	lseek(fd, port,SEEK_SET);
	write(fd, &value, 1);	
	close(fd);
//	return ret;
}
