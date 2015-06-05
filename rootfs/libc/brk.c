#include "../include/syscall.h"
#include "../include/stdlib.h"
int brk(void *end_data_segment)
{
	//if error return -1 else 0
	uint64_t *result;
	uint64_t temp = (uint64_t)end_data_segment;
	 __asm__ __volatile__(
                 "syscall"
                 : "=a" (result)
                 : "a"(SYS_brk), "D"(temp)
                );
	if(result==end_data_segment)
	{
		errno=12;//ENOMEM
		return -1;
	}
	else
		return 0;

}
