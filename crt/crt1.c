int errno = 0;

#include <stdlib.h>
#include <stdio.h>
int main(int argc, char* argv[], char* envp[]);

void _start(void) {
	int argc = 0;
       char** argv;
       char** envp;
        int res;

       uint64_t temp;
       __asm__ __volatile__ ("mov %%rsp, %0;"
                       :"=r"(temp)
       );

	printf("");
       while((*((uint64_t *)temp)) == 0)
               temp += sizeof(uint64_t) ;

       argc = *((uint64_t *)(temp));
         temp += sizeof(uint64_t) ;
       argv = (char**)(temp);
        temp += (sizeof(uint64_t) * (argc+1));
        envp = (char **)(temp);
       res = main(argc, argv, envp);
      exit(res);
}
