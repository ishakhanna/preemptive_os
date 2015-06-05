#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/defs.h>
#include <sys/syscall.h>

static __inline uint64_t syscall_0(uint64_t n) {
         uint64_t ret;
 /*       __asm__ __volatile__(
        "syscall"
        : "=a"(ret)
        : "a"(n)
);*/
__asm__ __volatile__( "movq %1,%%rax\n\t"
                  "int $128\n\t"
                   "movq %%rax, %0\n\t"
                   :"=r"(ret):"r"((uint64_t)n):"rax","memory");
        return ret;
}

static __inline uint64_t syscall_1(uint64_t n, uint64_t a1) {
	uint64_t ret;
/*	__asm__ __volatile__(
        "syscall"
        : "=a"(ret)
        : "a"(n), "D"(a1)
);*/
__asm__ __volatile__( "movq %1,%%rax\n\t"
                   "movq %2,%%rdi\n\t"
                   "int $128\n\t"
                   "movq %%rax, %0\n\t"
                   :"=r"(ret):"r"((uint64_t)n),"r"((uint64_t)a1):"rax","rdi","memory");
return ret;
}

static __inline uint64_t syscall_2(uint64_t n, uint64_t a1, uint64_t a2) {
       uint64_t ret;
    /*    __asm__ __volatile__(
        "syscall"
        : "=a"(ret)
        : "a"(n), "D"(a1), "S"(a2)
);*/
__asm__ __volatile__( "movq %1,%%rax\n\t"
                   "movq %2,%%rdi\n\t"
                   "movq %3,%%rsi\n\t"
                  "int $128\n\t"
                   "movq %%rax, %0\n\t"
                   :"=r"(ret):"r"((uint64_t)n),"r"((uint64_t)a1),"r"((uint64_t)a2):"rax","rdi","rsi","memory");
return ret;
}

static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	
       uint64_t ret;
/*__asm__ __volatile__(
        "syscall"
        : "=a" (ret)
        : "a"(n), "D"(a1), "S"(a2), "d"(a3)
);*/
__asm__ __volatile__( "movq %1,%%rax\n\t"
                   "movq %2,%%rdi\n\t"
                   "movq %3,%%rsi\n\t"
                   "movq %4,%%rdx\n\t"
                   "int $128\n\t"
		   "movq %%rax, %0\n\t"
                   :"=r"(ret):"r"((uint64_t)n),"r"((uint64_t)a1),"r"((uint64_t)a2),"r"((uint64_t)a3):"rax","rdi","rsi","rdx","memory");
return ret;
}

#endif
