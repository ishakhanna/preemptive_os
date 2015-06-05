#include <stdio.h>
#include <stdlib.h>
#include "utilities.h" 

char * get_data_from_input()
{
	char * data = (char *) malloc(sizeof(char) * 1000);
	read(0, data, 1000);
	return data;
}

char* StrStr(const char *haystack, const char *needle)
{
  if (!*needle)
	 return (char *)haystack;
  char *p1 = (char*)haystack, *p2 = (char*)needle;

  char *p1Adv = (char*)haystack;

  while (*++p2)
    	p1Adv++;
  while (*p1Adv)
  {
    char *p1Begin = p1;
    p2 = (char*)needle;
    while (*p1 && *p2 && *p1 == *p2)
    {
      p1++;
      p2++;
    }

    if (!*p2)
      return p1Begin;
    p1 = p1Begin + 1;
    p1Adv++;
  }
  return NULL;
}

int main(int argc, char * argv[], char * envp[])
{
	printf("\n---Inside grep---\n");
	if(argc == 1)
	{
		printf("Need an argument to search\n");
	}
	else
	{
		/* Take only the first argument */

		char * search_string = argv[1];
	//	printf("Word to search: %s\n", argv[1]);
		char * data = get_data_from_input();
		//printf("Data got from input:%s#\n", data);
		int i = 0;

		while(data[i] != '\0')
		{
			int j = i;
			int k = 0;
			char * word = (char *)malloc(100);

			while(data[j] != ' ' && data[j] != '\0' && data[j] != '\n')
			{
				word[k] = data[j];
				k++;
				j++;
			}
			word[k] = '\0';

			char * ans = StrStr(word, search_string);

			if(ans != NULL)
			{
				//printf("GREP OUTPUT: ###%s###\n", word);
				write(1, word, strlen(word));
				write(1, "\n", 1);
			}

			j++; // skip the space.

			i = j;
		}
	}
	return 0;
}
