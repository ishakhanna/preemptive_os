#include "sys/defs.h"

/* returns the length of the string */
int strlen(const char *s)
{
        int i = 0;
        while(s != NULL && s[i] != '\0')
                i++;
        return i;
}

/* Compares 2 strings for equality. */
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

char *strncpy(char *des, const char *src, uint64_t n)
{
    uint64_t i;

   for (i = 0; i < n && src[i] != '\0'; i++)
        des[i] = src[i];

    for ( ; i < n; i++)
        des[i] = '\0';

   return des;
}

void *memcpy(void *dest, void *src, uint64_t n){
    char *d = (char *)dest;
    char *s = (char *)src;
    for(int i = 0; i< n;i++){
        d[i] = s[i];
    }
    return dest;
}
char *itoa(int num)
{
    int i,rem = 0, strlen=0;
    int isNegative = 0;
    int num_cpy;

    if (num < 0)
    {
        isNegative = 1;
        num = -num;
    }

    // Counting string length to allocate memory to the char array
    for(num_cpy = num; num_cpy != 0; num_cpy /= 10) strlen++;

    if(isNegative) strlen++;

    static char str[20];

    i = strlen-1;

    while (num != 0)
    {
        rem = num % 10;
        str[i--] = rem + '0';
        num = num/10;
    }

    // If number is negative, append '-' to the start
    if (isNegative)
        str[i--] = '-';

    str[strlen] = '\0'; // Append string terminator
   
    //strcpy(result, str);

  //  return result;
	return str;
}

/*  convert a string to number */
int  atoi(char *str)
{
    int res = 0;
    int i;
    for (i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    return res;
}

/* Given a octal number convert into decimal number */

int oct_to_dec(int n)
{

  int res = 0;
  int i=1;
  while(n)
  {
    res = res + ((n%10)*i);
    i = i*8;
    n = n/10;
  }
  return res;
}

char *ltoa(uint64_t num)
{
    uint64_t i,rem = 0, strlen=0;
    uint64_t num_cpy;

    // Counting string length to allocate memory to the char array
    for(num_cpy = num; num_cpy != 0; num_cpy /= 10) strlen++;

    static char str[20];

    i = strlen-1;

    while (num != 0)
    {
        rem = num % 10;
        str[i--] = rem + '0';
        num = num/10;
    }

    str[strlen] = '\0'; // Append string terminator

    //strcpy(result, str);

   // return result;
	return str;
}

char *itohex(int num){
    int len = 0, rem = 0, i, num_cpy = 0;
    char c = '\0';

    for(num_cpy = num; num_cpy != 0; num_cpy /= 16) len++;

    static char hex[20];

    i = len-1;

    for(;num != 0; num/=16){
        rem = num % 16;
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
        hex[i--] = c;
    }
    hex[len] = '\0';// Append string terminator
    
//    strcpy(result, hex);

  //  return result;
	return hex;
}

char *ultohex(unsigned long num){
    int len = 0, rem = 0, i;
    unsigned long num_cpy = 0;
    char c = '\0';
    for(num_cpy = num; num_cpy != 0; num_cpy /= 16) len++;
    
    len = len+2; // To include 0x in the start
    static char hex[20];
    
    i = len-1;
    
    for(;num != 0; num/=16){
        rem = num % 16;
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
        hex[i--] = c;
    }

    hex[i--] = 'x';
    hex[i--] = '0';
    hex[len] = '\0';// Append string terminator
     
 //   strcpy(result, hex);
   // return result;
    return hex;
}

void *memset(void *s, int c, uint64_t n)
{
    unsigned char* p = s;

    while(n--)
        *p++ = (unsigned char) c;

    return s;
}
char *parse_execv_file(char *filename){
//	int len = strlen(filename);
//	char *res = (char *)kmalloc1(len);
//	memset(res, 0, len);
	if(filename[0] == '/'){
//		strcpy(res, filename+8);
		return filename+8;
	}
	else
		return filename;
}
