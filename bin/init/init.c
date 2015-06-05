#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[] ){
printf("In init\n");
char *arg_v[]= {"bin/hello1", NULL};
char *env_p[] = {"PATH=/rootfs/bin","CWD=/rootfs",NULL};
 int pid=fork();
    if (pid == 0) {
        execve("bin/hello1", arg_v,env_p);
    } else {
        waitpid(pid, 0, 0);
    }
    return 1;
}
