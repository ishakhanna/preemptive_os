#include <stdlib.h>
#include <stdio.h>

int  atoi(char *str);
int main(int argc, char* argv[]) 
{
  if(argc != 3){
	printf("Wrong Number of Arguments\n");
	return 0;
  }
  int pid = atoi(argv[2]);
  if(!kill(pid))
	printf("Successfully killed %d\n", pid);  
  else
	printf("%d : No such process\n", pid);
  return 0;
}

