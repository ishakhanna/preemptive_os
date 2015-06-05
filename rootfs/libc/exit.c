#include "../include/sys/syscall.h"
#include "../include/syscall.h"
#include "../include/stdlib.h"

void exit(int status){
syscall_1(SYS_exit, (uint64_t)status);
}
