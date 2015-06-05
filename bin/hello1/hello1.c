#include <stdio.h>
#include <stdlib.h>
#include "string_helper.h"
#include <string1.h>


int main(int argc, char ** arg1, char ** envp)
{
		char cmd[][20] = {"/rootfs/bin/ls",	"/rootfs/bin/grep","/rootfs/bin/grep"}; //,	"cat",	"wc"};
		char * argv[][3] = {{"bin/ls", NULL}, {"bin/grep", "lib", NULL},{"bin/grep", "c", NULL}};

		int fd[2];
		int pipelevel = 3;
		int i=0;
//		int status[3];

		for(i = 0;i < pipelevel;i++)
		{
			pipe(fd);
			int pid = fork();

			if(pid != 0)
			{
				//close(fd[1]);
				dup2(fd[0],0);
				//close(fd[0]);

				waitpid(pid, 0, 0);

				//execlp(cmd[pipelevel-i-1],cmd[pipelevel-i-1],(char*)0);
				execve(cmd[pipelevel-i-1], argv[pipelevel - i -1], envp);

			}
			else
			{
				if(i == (pipelevel - 1))
					exit(0);
				//close(fd[0]);
				dup2(fd[1],1);
				//close(fd[1]);
			}
		}
}
	/* THREE COMMANDS WORKING
		int fd_a[2];
		int fd_b[2];

		char * argv[] = {"bin/ls", NULL};
		char * argv1[] = {"bin/grep","lib", NULL};
		char * argv2[] = {"bin/echo","nitgarg", NULL};


		int pid = fork();

		if(pid == 0)
		{
			printf("Inside child-1\n");
			pipe(fd_a);

			int pid1 = fork();

			if(pid1 == 0)
			{
				printf("Inside child2\n");

				pipe(fd_b);

				int pid2 = fork();

				if(pid2 == 0)
				{
					printf("Inside child-3\n");

					dup2(fd_b[0], 0);
					execve("bin/echo", argv2, NULL);
					printf("Child-3 done\n");
					exit(0);
				}
				else
				{
					dup2(fd_a[0], 0);
					dup2(fd_b[1], 1);
					//dup2(fd_b[1], 1);
					execve("bin/grep", argv1, NULL);
					waitpid(pid2, 0, 0);
					exit(0);
				}
			}
			else
			{
				dup2(fd_a[1], 1);
				execve("bin/ls", argv, NULL);
				printf("Child 1 waiting for child2\n");
				waitpid(pid1, 0, 0);
				exit(0);
			}
		}
		else
		{
			printf("Waiting for child-1\n");
			waitpid(pid, 0, 0);
		}
	}
*/

/*  TWO COMMANDS WORKING CODE

		int fd[2];
		char * argv[] = {"bin/ls", NULL};
		char * argv1[] = {"bin/grep","lib", NULL};


		int pid = fork();

		if(pid == 0)
		{
			printf("Inside child\n");

			pipe(fd);

			int pid1 = fork();

			if(pid1 == 0)
			{
				printf("Inside child2\n");

				dup2(fd[0], 0);

				execve("bin/grep", argv1, NULL);
				exit(0);

			}
			else
			{
				dup2(fd[1], 1);

				execve("bin/ls", argv, NULL);
				printf("inside child1\n");
				waitpid(pid1, 0, 0);
			}
		}
		else
		{
			printf("Inside parent\n");
			waitpid(pid, 0, 0);
		}
}

*/

/*
	printf("Inside hello1\n");
			int pipefd[2];
			pipe(pipefd);

			int cpid = fork();
			if (cpid == -1)
			{
			   printf("fork");
			   exit(0);
			}

			if (cpid == 0)
			{
				printf("Coming here?\n");
				   close(pipefd[1]);


						   int fd[2];
						   pipe(fd);

						   char buf[10];
						   read(pipefd[0], buf, 10);
						   printf("Value received: %s\n", buf);


						   int ppid = fork();

						   if(ppid == -1)
						   {
							   printf("Error\n");
						   }

						   if(ppid == 0)
						   {
							   // Second child
							   printf("Second child\n");
							   char data[10];
							   close(fd[1]);
							   read(fd[0], data, 10);

							   printf("Received: %s\n", data);
							   while(1);
						   }
						   else
						   {
							   printf("Same here\n");
							   close(fd[0]);
							   write(fd[1], "Differentmessage ", 10);
							   close(fd[1]);
							   //while(1);
						   }

				   close(pipefd[0]);
				   exit(0);

			           } else {            // Parent writes argv[1] to pipe
			               close(pipefd[0]);  //         Close unused read end
			               //write(pipefd[1], "muralikrishna", 10);
			               dup2(pipefd[1], 1);
			               //printf("Wrote to buffer\n");
			               close(pipefd[1]);    //       Reader will see EOF
			               //wait(NULL);                Wait for child
			               //while(1);
			          }
	}

*/

	/*int fd[2];
	char * cmd[] = {"bin/echo", "bin/grep"};
	char * arg_echo[] = {"bin/echo", "anything", NULL};
	char * arg_grep[] = {"bin/grep", "any", NULL};

	pipe(fd);

	int id = fork();

	if(id == 0)
	{
		printf("In child\n");
		close(fd[1]);
		dup2(fd[0], 0);
		close(fd[0]);

		execve(cmd[1], arg_grep, NULL);
	}
	else
	{
		printf("In parent\n");
		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);

		execve(cmd[0], arg_echo, NULL);
		waitpid(id, 0, 0);
	}

*/





/*



	char cmd[][10] = {"bin/grep", "bin/echo"};
	int fd[2];
	int pipelevel = 2;
	int i=0;
	int status[2];
	char * arg[] = {"bin/echo","anything",NULL};

	for(i = 0;i < 2;i++)
	{
		pipe(fd);

		//printf("Descriptors: %d,%d\n", fd[0], fd[1]);

		int val = fork();
		if(val != 0)
		{
			close(fd[1]);
			dup2(fd[0],0);
			close(fd[0]);

			//wait(&status[i]);
			waitpid(val, 0, 0);

			//execlp(cmd[pipelevel-i-1], cmd[pipelevel-i-1], (char*)0);
			execve(cmd[pipelevel -i- 1], arg, NULL);
			//while(1);
		}
		else  Child
		{
			if(i == 1)
				exit(0);

			close(fd[0]);
			dup2(fd[1],1);
			close(fd[1]);
//exit(0);
//			/while(1);
		}
	}
*/

	//while(1);

/*

int fda[2];

pipe(fda);

int pid1 = fork();

if(pid1 == 0)
{
	// child-1
	close(fda[1]);
	char * data = (char *)malloc(10 * sizeof(char));
	dup2(fda[0], 0);
	close(fda[0]);
	//read(0, data, 10);
	printf("In child executing grep");

	execve("/rootfs/bin/ls", argv, envp);

//	/printf("child-1: received this message from parent:%s\n",data);

}
else
{
	//parent
	//write(fda[1], "mymessage1", 10);
	close(fda[0]);
	dup2(fda[1], 1);
	close(fda[1]);

	//printf("In parent executing ls\n");
	//execve("/bin/ls", argv, envp);

	//printf("parent: sent the message:'mymessage1' to child-1\n");
	//waitpid();
	//while(1);
	exit(0);
}

while(1);
return 0;
}
*/

/*


int abc(char *cmd,int inputFd,  char * PATH, char **envp){
	    int j,m;
	    char *c1, *c2;
	    char **c1_arr;
	    int fd[2];
	    j=m=0;

	    printf("Command given: %s\n", cmd);

	    while(cmd[j] != '|' && cmd[j] != '\0')
	       j++;

	    //printf("Here\n");
	    printf("Command given0: %s\n", cmd);
	    c1 = (char *)malloc(sizeof(char)*(j+1));
	    printf("Command given1: %s\n", cmd);
	    memset(c1, 0, j);
	    //printf("Command given2: %s\n", cmd);
	    strncpy(c1, cmd, j);
	    //printf("COMMAND EXECUTED :%s\n", cmd);
	    c2 = (char *)malloc(1024);
	    memset(c2, 0, sizeof(1024));
	    strcpy(c2, cmd+j+1);

	    c1_arr = mystrtok(c1, ' ');
	    printf("Here 4\n");

	    // while(c2[m] != '\0' && c2[m] != '|')
	      //      m++;

	        // Executing the Last command.
	    if(cmd[j] == '\0')
	    {
	        c1_arr = mystrtok(c1, ' ');

	       printf("Here 5\n");
	        //close(fd[1]);
	        if(inputFd != 0)
	            dup2(inputFd, 0);
		//    close(inputFd);
        	//printf("HERE!!!!!!!!!!!!!!!!!!!!!!!!!%s\n",c1_arr[0]);
	        int is_absolute = is_absolute_path(*c1_arr);
	        //printf("Here 6\n");
	        if(is_absolute)
	        {

	            int err=execve(*c1_arr, c1_arr, envp);
	            if(err==-1)
				{
				//printf("%s\n","ERROR!!!");
				return -1;
				}
		        //printf("Here 7\n");
		}
	        else
	        {
	           char *abs_path = get_absolute_path(*c1_arr, PATH);
		        //printf("Here 8\n");
	            if(abs_path == NULL){
	                return -111;
	            }
	            else
	            {

	                int err=execve(abs_path, c1_arr, envp);
			 if(err==-1)
	            	 {
	              //  	printf("%s\n","ERROR!");
	                	return -1;
	                 }
		    }

	        }
	        printf("Here 9\n");
	    }
	    else{

	        pipe(fd);
	        printf("Forking!!\n");
	        pid_t child_pid = fork();

	        if(child_pid == -1)
	        {
				printf("%s\n","ERROR");
				return -1;
	        }

	        else if(child_pid == 0){
printf("Inside child\n");
	        close(fd[0]);
		    if(inputFd!=0)
	        	dup2(inputFd, 0);
		//close(inputFd);
	        dup2(fd[1],1);
	        close(fd[1]);
	        printf("Here 10\n");

	            int is_absolute = is_absolute_path(*c1_arr);

	            if(is_absolute)
	            {
	                int err=execve(*c1_arr, c1_arr, envp);
			 if(err==-1)
	            	{
	                	printf("%s\n","ERROR");
	                	return -1;
	                }
			 printf("Here 11\n");
	            }
	            else
	            {
	                char *abs_path = get_absolute_path(*c1_arr, PATH);
	                printf("Here 12\n");
	                if(abs_path == NULL){
	                    return -111;
	                }

	                else
			{
	                        int err=execve(abs_path, c1_arr, envp);
			         if(err==-1)
	            		{
	                		printf("%s\n","ERROR");
	                		return -1;
	                	}
			         printf("Here 14\n");
			}
	            }


	        }
	        else{
	            close(fd[1]);
	            close(inputFd);
	            abc(c2,fd[0], PATH, envp);
	        }
	    }
	    return 0;
}


int main(int argc, char* argv[], char* envp[] )
{

	printf("Inside process 1\n");
	char * PATH = "/rootfs/";
	char *cmd = "bin/ls|bin/grep libc";



	abc(cmd, 0 , PATH, envp);



*/


/*

	char cmd[][10] = {"ls","cat","cat","cat","wc"};
		int fd[2];
		int pipelevel = 5;
		int i=0;
		int status[5];

		for(i = 0;i < 5;i++)
		{
			pipe(fd);
			if(fork() != 0) Parent
			{
				close(fd[1]);
				dup2(fd[0],0);
				close(fd[0]);

				waitpid(&status[i]);

				execve(cmd[pipelevel-i-1], cmd[pipelevel-i-1], (char*)0);
			}
			else  Child
			{
				if(i == 4)
				exit(0);
				close(fd[0]);
				dup2(fd[1],1);
				close(fd[1]);
			}
		}
		while(1);
*/
//		return 0;
//}
	/*int pid = fork();
printf("Back from 1st fork");
if(pid == 0){
	printf("In child of parent1(hello1)\n");
	execve("bin/hello", NULL, NULL);

}else{
	printf("In parent(hello1) with child pid %d\n", pid);
	//yield();
	printf("Back in parent1(hello1)\n");
}
while(1);
return 0;
}*/
