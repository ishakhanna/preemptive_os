int execute_script(char *filename, char *envp[]);

char *get_interpretor(char *filename){
	
    int fd= open(filename,O_RDONLY);
    // if error in opening 
    char buffer;
    char symbols[2];
    char *interpretor = (char *)malloc(1024); 
    int i=0;
    memset(interpretor, 0, 1024);
    if(fd<0)
    {
         printf("Error in opening file\n");
         return NULL;
    }
    if(read(fd,symbols,2)==0 || symbols[0]!='#' || symbols[1]!='!')
    {
        printf("Interpreter not mentioned in the script\n");
        return NULL;
    }	
    while(1)
    {
	read(fd,&buffer,1);
	if(buffer=='\n')
	   break;
	interpretor[i]=buffer;
	i++;	
    }
    interpretor[i]='\0';
    close(fd);
    return interpretor;
}

int process_binaries(char *input, char **envp){
    int i = 0;
    char key[50];
    char *new_path;
    char *cur_dir;
    
    /* Initialize new path as "PATH=<value of path>" */
    new_path = (char *)malloc((int)strlen(PATH)+5);
    memset(new_path, '\0', (int)strlen(PATH)+5);
    strcpy(new_path, "PATH=");
    strcpy(new_path+5, PATH);
    
    cur_dir = (char *)malloc((int)strlen(CWD)+4);
    memset(cur_dir, '\0', (int)strlen(CWD)+4);
    strcpy(cur_dir, "PWD=");
    strcpy(cur_dir+4, CWD);
    
    /* Update environmental variables PATH and PWD to current PATH and CWD */
    i = 0;
    while(envp[i] != NULL)
    {
        memset(key, '\0', 50);
        getKey(envp[i], key);
        
        if(strcmp(key, "PATH") == 0)
            envp[i] = new_path; // assign our PATH
        else if(strcmp(key, "PWD") == 0)
            envp[i] = cur_dir;
        i++;
    }
    
    i=0;
    while(input[i] != '\0' && input[i] != '|')
        i++;
    
    if(input[i] == '|'){
        execute_binary_with_pipe(input, envp);
    }
    else
        execute_script(input, envp);
   
    return 0;
}

int myexecute(char *filename,char **argv,char **envp){
    int err, status;
    pid_t pid = fork();
    if(pid == -1)
    {
        // Error creating process
        return -1;
    }
    else if(pid == 0)
    {
        err = execve(filename, argv, envp);
        if(err == -1)
        {
           // printf("%s\n",ERROR_MSG[errno-1]);
            exit(0);
        }
    }
    else
    {
        waitpid(pid, &status, 0);
        return 0;
    }
    return 0;
}

int execute_script(char *filename, char *envp[]){
    int status;
    int i;
    char full_filepath[500];
    char **argv;
    
    argv=mystrtok(filename, ' ');

/*
	Check if filename has '/' in it, then run the script directly. Otherwise 
	append PATH to the file name and check open(), if it returns -1, then try next PATH.	
*/
	int is_absolute = is_absolute_path(argv[0]);

    if(is_absolute)
	{
        myexecute(argv[0], argv, envp);
	}
	else
	{
		/* Need to append PATH and check if the file is present using open, if present, run it. */
        char *abs_path = get_absolute_path(argv[0]);
        if(abs_path == NULL){
            return -MYENOCMD;
        }
        else
            myexecute(abs_path, argv, envp);
	}
    return 0;
}

int execute_sbush_script(char *filename, char **envp){
    char *line;
    char c;
    int ignoreline, i = 0;
    ignoreline = 0;
    int fd= open(filename,O_RDONLY);
    
    if(fd<0)
        //return errno;
    
        return -1;
    line = (char *)malloc(1024);
    memset(line,0,1024);
    
    read(fd,&c,1);
    while(TRUE){
        if(c == '#')
            ignoreline = 1;
        while(c != '\n' && c != '\0'){
            line[i] = c;
            c = '\0';
            int a = read(fd,&c,1);
            i++;
        }
        if(!(strlen(line) <= 0) || (ignoreline == 1)){
            line[i] = '\0';
         
            execute_script(line, envp);
        }
        if(c == '\0')
            break;
        if((c == '\n') && (read(fd,&c,1)==0))
            break;

        // Resetting Loop invariants
        memset(line, 0, 1024);
        i=0;
    }
    return 0;
}
