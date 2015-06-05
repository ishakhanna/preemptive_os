int errno = 0;
#include "sys/defs.h"
char *PS1;
char *PATH;
char *CWD;
char *COMMANDS[] = {"exit", "help", "cd", "prompt", "setpath", "echo", NULL};

#include "utilities.h"

void _start(void) {
	int argc = 0;
	char* argv[10];
	char* envp[0];
//	int res;

int *temp;
__asm__ __volatile__ ("mov %%rsp, %0;"
        :"=r"(temp)
);
while(*(temp) == 0)
	temp++;
//temp--;
argc = *temp;
temp++;
//argv = temp;
/*while(*(temp) != 0){
	*argv = *(temp);
	argv++;
	temp++;
}
*argv = NULL;
temp++;
while(*(temp) != 0){
        envp++ = temp++;
}
envp = NULL;
*/
printf("a\n");

//	__asm__ __volatile__ ("mov temp, %rsp");
//	printf("------\n");
//	res = main(argc, argv, envp);
	main(argc, argv, envp);
//	exit(res);
	exit(0);
}
