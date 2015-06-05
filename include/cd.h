int changeDirectory(char *dest, char **envp){
    int i = 0;
    char *key = (char *)malloc(50);
    char *oldpwd = NULL;
    if(strcmp(dest, "-") == 0){
        while(envp[i] != NULL)
        {
            memset(key, '\0', 50);

            getKey(envp[i], key);
        
            if(strcmp(key, "OLDPWD") == 0){
                oldpwd = (char *)malloc(sizeof(envp[i]));
                strcpy(oldpwd, envp[i]+7);
		printf("--%s--\n", oldpwd);
		break;
            }
            i++;
        }
	if(oldpwd != NULL)
	{
		int ret=chdir(oldpwd);
		if(ret==-1)
		{
			printf("%s\n",ERROR_MSG[errno-1]);
			return -1;
		}
		else
			return 0;
	}
	printf("cd: OLDPWD not set\n");
	return -1;
    }
    else
	{
		 int ret=chdir(dest);
                if(ret==-1)
                {
                        printf("%s\n",ERROR_MSG[errno-1]);
                        return -1;
                }
                else
                        return 0;			
	}
}
