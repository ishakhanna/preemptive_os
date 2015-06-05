void displayHelp()
{
        printf("Available commands: \n\n");
        printf("cd\t\tChange the directory. Usage:cd <directory_to_go_into>\n");
        printf("prompt\t\tChange the prompt. Usage:prompt <prompt_name>\n");
        printf("exit\t\tExit sbush shell. Usage:exit\n");
        printf("help\t\tDisplay available commands. Usage:help\n");
	printf("setpath\t\tSet the path environment variable. Usage:setpath <path_to_set>\n");
	printf("echo\t\tDisplay the environmental variable. Usage:echo $<env_variable>\n");
        printf("\n");
}
