/* returns the length of the string */
int strlen(const char *s)
{
        int i = 0;
        while(s != NULL && s[i] != '\0')
                i++;
        return i;
}

/* Comapres 2 strings for equality. */
int strcmp(const char *s1, const char *s2)
{
        if(s1 == NULL || s2 == NULL)
                return -1;

        int i = 0;
        while(s1[i] != '\0' && s2[i] != '\0')
        {
                if(s1[i] != s2[i])
                        return ((int) s1[i] - (int) s2[i]);
                i++;
        }

        if(s1[i] == '\0' && s2[i] == '\0')
                return 0;

        if(s1[i] == '\0')
                return (-1) * (int) s2[i];
        else
                return (int) s1[i];
}

char *strcpy(char *des, const char *src)
{
	int i = 0;
	while(src[i] != '\0')
	{
		des[i] = src[i];
		i++;
	}
	des[i] = '\0';

	return des;
}

char *strncpy(char *des, const char *src, size_t n)
{
    size_t i;

   for (i = 0; i < n && src[i] != '\0'; i++)
        des[i] = src[i];

    for ( ; i < n; i++)
        des[i] = '\0';

   return des;
}

/* Comapres 2 strings for equality. */
int strncmp(const char *s1, const char *s2, int len)
{
        if(s1 == NULL || s2 == NULL)
                return -1;

        int i = 0;
        while(s1[i] != '\0' && s2[i] != '\0' && i < len)
        {
                if(s1[i] != s2[i])
                        return ((int) s1[i] - (int) s2[i]);
                i++;
        }

        if(i == len)
        {
                return 0;
        }

        if(s1[i] == '\0' && s2[i] == '\0')
                return 0;

        if(s1[i] == '\0')
                return (-1) * (int) s2[i];
        else
                return (int) s1[i];
}

void *memcpy(void *dest, void *src, size_t n){
    char *d = (char *)dest;
    char *s = (char *)src;
    for(int i = 0; i< n;i++){
        d[i] = s[i];
    }
    return dest;
}

/* Find sthe substring */
char* strstr(char *str1, char *str2)
{
	if (!*str2)
	{
		return str1;
	}

	char *tmp = (char*) str1;
	
	while(*tmp)
	{
		char *tmpBegin = tmp, *p = (char*)str2;

		while (*tmp && *p && *tmp == *p)
		{
			tmp++;
			p++;
		}

		if (!*p)
		{
			return tmpBegin;
		}

		tmp = tmpBegin + 1;
	}
	return NULL;
}

void reverse(char *str, int len){
    int j = len - 1, i = 0;
	while(i < j)
	{
		char c = str[i];
		str[i] = str[j];
		str[j] = c;
		i++;
		j--;
	}
}

void *memset(void *s, int c, size_t n)
{
    unsigned char* p = s;

    while(n--)
        *p++ = (unsigned char) c;

    return s;
}

char* itoa(int num)
{
    int i = 0;
    int isNegative = 0;
    char *str = (char *)malloc(100 * sizeof(char)); //[100];
    if (num == 0)
    {
        str[i++] = '0';
        str[i++] = '\0';
        return str;
    }
    if (num < 0)
    {
        isNegative = 1;
        num = -num;
    }
    while (num != 0)
    {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num/10;
    }
    
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
    
    str[i] = '\0'; // Append string terminator
    
    // Reverse the string
    reverse(str, i);
    
    return str;
}

char *itohex(uint64_t a){
    uint64_t len = 0, rem = 0, i=0;
    char c = '\0';
    uint64_t num = a;
    for(;num != 0; num /= 10)
        len++;
        
    char *hex = (char *)malloc(len);
    
    for(;a != 0; a/=16){
        rem = a % 16;
        if(rem >= 10){
            switch (rem) {
                case 10:
                    c = 'a';
                    break;
                case 11:
                    c = 'b';
                    break;
                case 12:
                    c = 'c';
                    break;
                case 13:
                    c = 'd';
                    break;
                case 14:
                    c = 'e';
                    break;
                case 15:
                    c = 'f';
                    break;
                default:
                    break;
            }
        }
        else
            c = '0'+rem;
        hex[i++] = c;
    }
    reverse(hex, i);
    return hex;
}
