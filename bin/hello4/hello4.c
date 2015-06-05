#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[], char* envp[] ){
printf("\nIN Hello\n");
char *a;
a = (char *)0xFFFFF00100000100;
printf("%s\n", a);
return 0;
}
