//char *init_path(char *envp[]){
//    int i = 0;
//    char *p = (char *) malloc(1000);
//
//    memset(p, 0, 1000);
//
//    while(envp[i] != NULL){
//        if(strncmp(envp[i], "PATH=", 5) == 0){
//            strcpy(p, envp[i]+5);
//	    break;
//	}
//
//        i++;
//    }
//
//    return p;
//}
void set_path(char **path, char *new_path)
{
	int i = 0, fin_len;
	int old_len = strlen(*path);
	int new_len = strlen(new_path);
    
    char *repl = strstr(new_path, "$PATH");
    
	if(repl == NULL)
	{
		memset(*path, 0, old_len);
		memcpy(*path, new_path, new_len);
	}
	else
	{
        fin_len = old_len+new_len;
        char value[fin_len];
        memset(value, 0, fin_len);
        i=0;
        while(new_path != repl){
            value[i] = new_path[0];
            i++;
            new_path++;
        }
	strncpy(value+i, *path, old_len);
	i += old_len;
        new_path += 5; // move 5 bytes ahead
        strcpy(value+i, new_path);

        memset(*path, 0, fin_len);
        memcpy(*path, value, fin_len);

    }
}
