char **mystrtok(char *str, char c){
    char **args;
    int no_of_tokens;
    int args_count,i,j;
    no_of_tokens = args_count = j = i = 0;
    
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
        
        *(args+args_count) = malloc((j-i)*sizeof(char));
        
        memset(*(args+args_count), 0, (j-i)*sizeof(char));
        
        strncpy(*(args+args_count), str+i, (j-i));
        
        args_count++;
        
        if(str[j] == '\0')
            break;
        j++;
        i=j;
    }
    *(args+args_count) = NULL;
    
    return args;
}

/* Figues out if the file name is absolute or relative */
int is_absolute_path(char *filename)
{
    /* If absolute path is given or script in current directory is given, return true */
    if(filename != NULL && (filename[0] == '/' || (filename[0] == '.' && filename[1] == '/')))
        return TRUE;
    else
        return FALSE;
}

char *get_absolute_path(char *filename){
    int i = 0, j = 0, no_of_args = 0;
    char **path_array;
    char *abs_path;
    
    path_array = mystrtok(PATH, ':');
    
    while(*(path_array+i) != '\0')
    {
        abs_path = (char *)malloc((int)strlen(path_array[i]) + (int)strlen(filename) + 1);
        memset(abs_path, '0', (int)strlen(path_array[i]) + (int)strlen(filename) + 1);
        strcpy(abs_path, path_array[i]);
        strcpy(abs_path + (int)strlen(path_array[i]), "/");
        strcpy(abs_path + (int)strlen(path_array[i]) + 1, filename);
        
        if(open(abs_path, O_RDONLY) != -1)
            return abs_path;
        i++;
    }
    return NULL;
}