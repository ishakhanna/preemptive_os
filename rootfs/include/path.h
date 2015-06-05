/*void init_path(char **path, char *envp[])
{
	int i = 0;
	char tmp[6];
	memset(tmp, 0, 6);

	while(envp[i] != NULL)
	{
		char *p = envp[i];
		memcpy(tmp, &p[0], 5);
		tmp[5] = '\0';

		if(strcmp("PATH=", tmp) == 0)
		{
			break;
		}		
		i++;
	}
	char *p = envp[i];
	memcpy(*path, &p[5], 1000);
}*/

char *init_path(char *envp[]){
    int i = 0;
    char *p = (char *) malloc(1000);
    
    memset(p, 0, 1000);
    
    while(envp[i] != NULL){
        
        if(strncmp(envp[i], "PATH=", 5) == 0)
            strcpy(p, envp[i]+5);
        
        i++;
    }
    
    return p;
}

void set_path(char **path, char *new_path)
{
	int i = 0;
	int append = FALSE;
	int old_len = strlen(*path);
	int new_len = strlen(new_path);

	if(strstr(new_path, "$PATH") != NULL)
		append = TRUE;

	if(append == FALSE)
	{
		memset(*path, 0, 1000);
		memcpy(*path, new_path, 1000);
	}
	else
	{
		int fin_len = new_len - 5; // minus $PATH
		char value[fin_len];
		memset(value, 0, fin_len);
		int k = 0;

		while(new_path[i] != '\0')
		{			
			if(new_path[i] == '$')
			{
				while(new_path[i] != ':')
				{
					i++;
				}
			i++;
			}			
		value[k] = new_path[i];
		i++;
		k++;
		}
		
		value[k] = '\0';
		(*path)[old_len++] = ':';		

		k = 0;
		while(value[k] != '\0')
		{
			(*path)[old_len++] = value[k++];
		}
		
		(*path)[old_len] = '\0';
	}
}
