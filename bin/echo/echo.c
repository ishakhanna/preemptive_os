#include <stdio.h>
#include <stdlib.h>
#include "string_helper.h"

char * search_for_value(char **, char *);
int contains(char * string, char * key);

int main(int argc, char* argv[], char *envp[])
{
  if(argc == 1){
        //printf("\n");
        return 0;
  }

  if(argc >= 2)
  {
	  /* Will only take first argument */
	  //printf("In echo: %s   ", argv[1]);
	  char * argument = argv[1];

	  if(argument[0] == '$')
	  {
		  char * value = search_for_value(envp, argument + 1);
		  write(1, value, strlen(value));
		  write(1, "\n", 1);
		  //printf("%s\n", value);
	  }
	  else
	  {
		int i = 1;
		while(i < argc)
		{
			write(1, argv[i], strlen(argv[i]));
			//printf("%s ", argv[i]);
			i++;
		}
		write(1, "\n", 1);
		//printf("\n");
	  }
  }

  return 0;
}

int contains(char * string, char * key)
{
	int i = 0;
	while(key[i] != '\0')
	{
		if(string[i] != key[i])
		{
			return -1;
		}
		i++;
	}

	if(string[i] == '=')
	{
		return i;
	}
	else
	{
		return -1;
	}
}

char * search_for_value(char * envp[], char * key)
{
	int i = 0;
	while(envp[i] != NULL)
	{
		int index = contains(envp[i], key);
		if(index >= 0)
		{
			char * result = (char *)malloc(sizeof(char) * 100);
			memset(result, '\0', 100);

			strcpy(result, envp[i] + (index + 1));
			return result;
		}
		i++;
	}
	return NULL;
}

