char *argenv[] = {"PATH=/bin", NULL};

int abc(char *cmd,int inputFd, char **envp){
    int status, j,m;
    char *c1, *c2;
    char **c1_arr, **c2_arr;
    int fd[2];
    j=m=0;

    while(cmd[j] != '|' && cmd[j] != '\0')
       j++;
    
    c1 = (char *)malloc(sizeof(char)*(j+1));
    memset(c1, 0, j);
    strncpy(c1, cmd, j);
    
    c2 = (char *)malloc(1024);
    memset(c2, 0, sizeof(1024));
    strcpy(c2, cmd+j+1);
    
    c1_arr = mystrtok(c1, ' ');

    // while(c2[m] != '\0' && c2[m] != '|')
      //      m++;
        
        // Executing the Last command.
    if(cmd[j] == '\0'){
        c1_arr = mystrtok(c1, ' ');
        
        //close(fd[1]);
        if(inputFd != 0)
            dup2(inputFd, 0);
	//    close(inputFd);
        
        int is_absolute = is_absolute_path(*c1_arr);
        
        if(is_absolute)
        {
            execve(*c1_arr, c1_arr, argenv);
        }
        else
        {
            /* Need to append PATH and check if the file is present using open, if present, run it. */
           char *abs_path = get_absolute_path(*c1_arr);
            if(abs_path == NULL){
                return -MYENOCMD;
            }
            else
                execve(abs_path, c1_arr, argenv);
        }

    }
    else{

        pipe(fd);
        pid_t child_pid = fork();
    
        if(child_pid == -1)
            return -1;
        else if(child_pid == 0){
       
        close(fd[0]);
	    if(inputFd!=0)
        	dup2(inputFd, 0);
	//close(inputFd);
        dup2(fd[1],1);
        close(fd[1]);
            
            
            int is_absolute = is_absolute_path(*c1_arr);
            
            if(is_absolute)
            {
                execve(*c1_arr, c1_arr, argenv);
            }
            else
            {
                /* Need to append PATH and check if the file is present using open, if present, run it. */
                char *abs_path = get_absolute_path(*c1_arr);
                if(abs_path == NULL){
                    return -MYENOCMD;
                }
                else
                    execve(abs_path, c1_arr, argenv);
            }
	
        }
        else{
            close(fd[1]);
            close(inputFd);
            abc(c2,fd[0], argenv);
        }
    }
    return 0;
}

int execute_binary_with_pipe(char *cmd, char **envp){
    int status;
    
    pid_t child_pid = fork();
    if(child_pid == -1){
        return -1;
    }
   else if (child_pid == 0){
        abc(cmd,0, envp);
    }
    else{
        waitpid(child_pid, &status, 0);
        return 0;
    }
    
    return 0;
}
