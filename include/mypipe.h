/* Maximum number of commands allowed in pipe */
int max_cmd_allowed = 10;

int get_commands_count(char * main_cmd)
{
	int no_of_cmds = 0;
	int i = 0;
	while(main_cmd[i] != '\0')
	{
		if(main_cmd[i] == '|')
		{
			no_of_cmds++;
		}
		i++;
	}
	return no_of_cmds + 1;
}

void get_arguments_for_command(char * cmd, int number_to_skip, char *argv[max_cmd_allowed])
{
	int i = 0;

	while(cmd[i] != '\0' && number_to_skip > 0)
	{
		if(cmd[i] == '|')
		{
			number_to_skip--;
		}
		i++;
	}

	int c1 = 0;
	int index = 0;
	argv[c1] = (char *)malloc(20 * sizeof(char));
	memset(argv[c1], 0, 20);

	while(cmd[i] != '\0')
	{
		if(cmd[i] != ' ' && cmd[i] != '|')
		{
			argv[c1][index] = cmd[i];
			index++;
			i++;
			continue;
		}
		else if(cmd[i] == ' ')
		{
			argv[c1][index] = '\0';
			c1++;

			argv[c1] = (char *)malloc(20 * sizeof(char));
			memset(argv[c1], 0, 20);

			index = 0;
			i++;
			continue;
		}
		else if(cmd[i] == '|')
		{
			argv[c1][index] = '\0';
			c1++;
			index = 0;
			break;
		}
	}

	if(cmd[i] == '|')
	{
		argv[c1] = (char *)NULL;
	}
	else if(cmd[i] == '\0')
	{
		argv[c1][index] = '\0';
		argv[c1 + 1] = (char *)NULL;
	}
}

void divide_and_conquer(char * cmd, char **envp, int no_of_commands, char ***main_arguments)
{
	int fd[2];
	int i;
	for(i = 0; i < no_of_commands; i++)
	{
		pipe(fd);

		int pid = fork();
		if(pid != 0)
		{
			close(fd[1]);
			dup2(fd[0],0);
			close(fd[0]);
			waitpid(pid, 0, 0);
			execve(main_arguments[no_of_commands - i - 1][0], main_arguments[no_of_commands - i - 1], envp);
    			printf("No such file or directory\n");
            		exit(0);
    
		}
		else
		{
			if(i == (no_of_commands - 1))
				exit(0);
			close(fd[0]);
			dup2(fd[1],1);
			close(fd[1]);
		}
	}
}

int check_valid(char *str)
{
    char **args;
    int no_of_tokens;
    int args_count,i,j;
    no_of_tokens = args_count = j = i = 0;
    char c = '|';
    while(str[i] != '\0'){
        if(str[i] == c)
            no_of_tokens++;
        i++;
    }
    
    args = malloc(sizeof(char *)*(no_of_tokens+2));
    
    i=0;
    
    while(str[i] != '\0')
    {
        while(str[j] != c && str[j] != '\0')
            j++;
	if(i == j){
		return -1;
	}
        *(args+args_count) = malloc((j-i)*sizeof(char));
        
        memset(*(args+args_count), 0, (j-i)*sizeof(char));
        
        strncpy(*(args+args_count), str+i, (j-i));
        
        args_count++;
        
        if(str[j] == '\0')
            break;
        j++;
        i=j;
    }
	if(str[j-1] == '|')
		return -1;

   return 0; 
    
}

int execute_binary_with_pipe(char *cmd, char **envp)
{
    if(check_valid(cmd) != 0)
    {
		printf("Invalid Null Command\n");
		return 0;
    }
    //printf("Command received: %s\n", cmd);
    int i;
    int no_of_commands = get_commands_count(cmd);
    //printf("No of commands:%d\n", no_of_commands);
    char **main_arguments[no_of_commands];
    for(i = 0; i < no_of_commands; i++)
    {
        char **argv = (char **)malloc(max_cmd_allowed * sizeof(char *));

        get_arguments_for_command(cmd, i, argv);
        main_arguments[i] = argv;
    }

    int pid = fork();
    if(pid == 0){
        divide_and_conquer(cmd,envp, no_of_commands, main_arguments);
        exit(0);
    }
    else{
        waitpid(pid, 0, 0);
    }

    return 0;
}
