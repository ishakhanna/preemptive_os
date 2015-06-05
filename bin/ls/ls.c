#include <stdio.h>
#include <stdlib.h>
#include "utilities.h" 

int main(int argc, char * argv[])
{
	if(argc < 1)
	{
		printf("Error in ls.c\n");
	}

	if(argc == 1)
	{
		/* list files or dir in current working dir */
		char directory[100];
		getcwd(directory, 100);

		//printf("No arguments given, so listing files in current directory: %s\n", directory);

		struct Dir * dir = (struct Dir *)opendir(directory);
		struct dirent * dentry ;

		while((dentry = readdir((void *)dir)) != NULL)
		{
			char data[100];
			strncpy(data, dentry->d_name, strlen(dentry->d_name));
			write(1, data, strlen(dentry->d_name));
			write(1, "\n", 1);
		}
	}

	if(argc >= 2)
	{
		/* Only first folder will be considered */

		//printf("Argument received: %s\n", argv[1]);
		char old_cwd[100];
		getcwd(old_cwd, 100);

		char * user_input = argv[1];
		int res = chdir(user_input);


		if(res == 0)
		{
			char new_cwd[100];
			getcwd(new_cwd, 100);

			struct Dir * dir = (struct Dir *)opendir(new_cwd);
			struct dirent * dentry;

			while((dentry = readdir((void *)dir)) != NULL)
			{
				char data[100];
				strncpy(data, dentry->d_name, strlen(dentry->d_name));
				write(1, data, strlen(dentry->d_name));
				write(1, "\n", 1);
			}
			/* Don't forget to reset current working directory */
			chdir(old_cwd);
		}
		else
		{
			//printf("%s: No such directory\n", user_input);
			char * data = "No such file or directory";
			write(1, data, 30);
		}
	}

	return 0;
}
