void getKey(char *env, char *key)
{
	int i = 0;
	while(env[i] != '=')
	{
		i++;
	}

	int k = 0;
	while(k < i)
	{
		key[k] = env[k];
		k++;
	}
}

void getValue(char *env, char *value)
{
	int i = 0;

	while(env[i] != '=')
	{
		i++;
	}
	i++;
	int k = 0;

	while(env[i] != '\0')
	{
		value[k] = env[i];
		i++;
		k++;
	}
}

void echo(char *envp[], char *cmd)
{
	int i = 0;
	int len = strlen(cmd);
	char key[50], value[500];

	while(envp[i] != NULL)
	{
		memset(key, '\0', 50);

		getKey(envp[i], key);
		if(strcmp(cmd, key) == 0)
		{
			memset(value, '\0', 500);
			getValue(envp[i], value);
			printf("%s\n", value);
			break;
		}
		i++;
	}
}

