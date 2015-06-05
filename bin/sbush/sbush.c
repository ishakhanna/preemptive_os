#include "utilities.h"

//char *PS1;
//char *PATH;
//char *CWD;

int get_command_id(char *input){
    char *cmd;
    char *COMMANDS[] = {"exit", "help", "cd", "prompt", "setpath", "echo", NULL};

    int i = 0, j = 0;
    
    int cmdId = -1;

    if(input == NULL || strlen(input) == 0)
        goto end_fn;
    
    while(input[i] != ' ' && input[i] != '\0')
        i++;

    cmd = (char *)malloc(MAX_LEN);
    memset(cmd, 0, MAX_LEN);

    strncpy(cmd, input, i);
    
    while(COMMANDS[j] != NULL){
        if(strcmp(COMMANDS[j], cmd) == 0)
            break;
        j++;
    }
    
    cmdId = j;

    if(cmdId == 6 && strcmp(cmd, "/bin/echo") == 0)
    {
	cmdId = 5; // echo
    }

    free(cmd);
    end_fn:
    return cmdId;
}

char *get_command_param(char *input, int cmdId){
    int i = 0;
    
    if(cmdId == CMD_CD ||cmdId == CMD_PROMPT ||cmdId == CMD_SET_PATH){
        while(input[i] != '\0' && input[i] != ' ')
            i++;
	if(input[i] == '\0')
		return NULL;
        return input+i+1;
    }

    if(cmdId == CMD_ECHO)
    {
	i = 0; // skip 'echo $'
	while(input[i] != '\0' && input[i] != '$' && input[i] != ' ')
	{
	    i++;
	}

        if(input[i] == '$' && input[i+1] != '\0')
            return input+i;
        
        if(input[i] == ' ' && input[i+1] != '\0')
            return input+i+1;
    }

    return NULL;
}

/* Parses command and calls appropriate function. */
int processInput(char *input, char *envp[], char *PATH, char *CWD, char *PS1)
{
        char *cmdParam;
        int resultCode = -1;
        int cmdId;
    
        cmdId = get_command_id(input);
    //printf("CMDiD is:%d\n", cmdId);
    
        switch(cmdId)
        {
            case CMD_EXIT :
		        //printf("Exit Command\n");
                resultCode = EXIT_NOERROR;
                break;

            case CMD_HELP :
                displayHelp();
                resultCode = CONTINUE;
                break;

            case CMD_PROMPT :
                cmdParam = get_command_param(input, cmdId);
                if(cmdParam != NULL)
                {
                    memset(PS1, 0, (MAX_LEN - sizeof("prompt ")));
                    strcpy(PS1, cmdParam);
                }
                resultCode = CONTINUE;
                break;
                
            case CMD_CD :
                cmdParam = get_command_param(input, cmdId);
                changeDirectory(cmdParam, envp);
                resultCode = CONTINUE;
                break;
            
            case CMD_EXEC_BIN :
      		    process_binaries(input, envp, PATH, CWD);
		//printf("Back in first SBUSH\n"); 
               resultCode = CONTINUE;
                break;
     
            case CMD_SET_PATH :
                cmdParam = get_command_param(input, cmdId);
                if(cmdParam != NULL)
                    set_path(&PATH, cmdParam);
                resultCode = CONTINUE;
                break;

            case CMD_ECHO :
                cmdParam = get_command_param(input, cmdId);
                if(cmdParam != NULL)
                {
                    if(cmdParam[0] == '$'){
                        cmdParam++;
                        if(strcmp(cmdParam, "PATH") == 0)
                            printf("%s\n", PATH);
                        else if(strcmp(cmdParam, "PS1") == 0)
                            printf("%s\n", PS1);
                        else if(strcmp(cmdParam, "PWD") == 0)
                            printf("%s\n", CWD);
                        else
                            echo(envp, cmdParam);
                    }
                    else{
                        printf("%s\n", cmdParam);
                    }
                }
		        resultCode = CONTINUE;
                break;
	
            default:
                printf("Invalid Command! (%s)\n\n", input);
                displayHelp();
                resultCode = CONTINUE;
        }
    
    return resultCode;
}

int main(int argc, char *argv[], char *envp[])
{
	printf("\n\n-------WELCOME TO SBUSH-------------\n\n");
     char *input;
      int ret_value;
    input = (char *)malloc(MAX_LEN);
    char *PS1 = (char *)malloc(MAX_LEN - sizeof("prompt "));
    char *CWD = (char *)malloc(200);
    
    memset(PS1, 0, MAX_LEN - sizeof("prompt "));
    strcpy(PS1, "sbush");

//    char *PATH = init_path(envp);

    /*INIT PATH*/
    int i = 0;
    char *PATH = (char *) malloc(1000);

    memset(PATH, 0, 1000);

    while(envp[i] != NULL){
        if(strncmp(envp[i], "PATH=", 5) == 0){
            strcpy(PATH, envp[i]+5);
        break;
    }
        i++;
    }

    if(argc > 1){
        memset(CWD, 0, 200);

        getcwd(CWD, 200);
	//printf("IN sbush got argc: %d and argv : %s\n", argc, argv[1]);
        execute_sbush_script(argv[1], envp, PATH, CWD);
	//printf("Finally exiting\n");
	exit(0);
    }
    else
    {
        printf("\nSBUSH - SBU Shell.\n\n");
        while(1)
        {
   	        memset(CWD, 0, 200);
        
            memset(input, 0, MAX_LEN);
	        getcwd(CWD, 200);
            printf("%s : %s# ", CWD, PS1);
            if(PATH[0] == '\0'){
                strcpy(PATH, "/rootfs/bin");
            }
//            printf("PAth: %s\n", PATH);
	        scanf("%s", input);
//	        printf("\nUser Entered ");
//	        printf("%s\n", input);
            printf("\n");
	        if(strlen(input) == 0)
                continue;

            ret_value = processInput(input, envp, PATH, CWD, PS1);
//		    printf("Cmd id returned: %d\n", ret_value);
	        if(ret_value == EXIT_NOERROR)
            {
                printf("Exiting sbush...\n");
                break;
            }
        }
    }
    
    free(input);
    free(CWD);
    free(PATH);
    
    return 0;

}
