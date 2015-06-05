int changeDirectory(char *dest, char **envp){
    int i = 0;
    char *key = (char *)malloc(50);
    char *oldpwd;
    if(strcmp(dest, "-") == 0){
	printf("\nYou enterd cd -\n");
        while(envp[i] != NULL)
        {
            memset(key, '\0', 50);

            getKey(envp[i], key);
        
            if(strcmp(key, "OLDPWD") == 0){
                oldpwd = (char *)malloc(sizeof(envp[i]));
                strcpy(oldpwd, envp[i]+7);
            }
            i++;
        }
        return chdir(oldpwd);
    }
    else
        return chdir(dest);
}
