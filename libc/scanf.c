#include "stdarg.h"
#include <stdlib.h>
#include <stdio.h>
#include "string_helper.h"

// stdin fd = 0

char read_char()
{
        char c;
        read(0,&c, 1);
        return c;
}

int get_hex_equivalent(char c)
{
        switch(c)
        {
                case 'a':
                case 'A':
                        return 10;

                case 'b':
                case 'B':
                        return 11;

                case 'c':
                case 'C':
                        return 12;

                case 'd':
                case 'D':
                        return 13;

                case 'e':
                case 'E':
                        return 14;

                case 'f':
                case 'F':
                        return 15;

                default:
                        break;
        }
        return ((int) c - 48);
}

int hex_to_int(char *hex, int len)
{
        int val = 0;
        int h = 1; // powers of 16 value
        int tmp = 0;

        while(len--)
        {
                tmp = get_hex_equivalent(hex[len]);
                val = val + tmp * h;
                h = h * 16;
        }
        return val;
}

int isDecimal(char c)
{
        int ret = 0;
        if(c >= 48 && c <= 57) //0 - 9
                ret = 1;
        return ret;
}

/*int read_int()
{
        int value = 0;
        char c;

        while(1)
        {
                c = read_char();
                if(c == ' '|| c == '\n')
                        break;
                else
                {
                        if(isDecimal(c))
                                value = value * 10 + ((int) c - 48);
                        else if(c == '\b')
                        {
                        	value = value / 10;
                        }
                        else
                        {
                                printf("Not a decimal\n");
                                break;
                        }
                }
        }

        return value;
}*/

char *read_str(char *str, int bytes)
{
        read(0, str, bytes);
        return str;
}

int read_int()
{
        int value = 0;
        char c[50];
        read_str(c, 50);
        value = atoi(c);
        return value;
}

int read_hex()
{
        char hex[100];
        int i = 0;
        char c;

        while(1)
        {
                c = read_char();
                if(c == ' '|| c == '\n')
                {
                        hex[i] = '\0';
                        break;
                }
                else if(c == '\b')
                {
			if(i > 0)
                		hex[--i] = ' ';
                }
                else
                {
                        hex[i] = c;
                        i++;
                }
        }

        return hex_to_int(hex, i);
}

int scanf(const char *format, ...)
{
        va_list val;
//        int scanned = 0;

        int *ip;
        char *cp;
        char *sp;

        va_start(val, format);
        while(*format)
        {
                if(*format == '%')
                {
                        ++format;
                        switch(*format)
                        {
                                case 'd':
                                        ip = va_arg(val, int *);
                                        *ip = read_int();
                                   //     scanned++;
                                        break;
                                case 'c':
                                        cp = va_arg(val, char *);
                                        char tmp;
                                        while((tmp = read_char()) == '\b')
                                        {
                                        	continue;
                                        }
                                        *cp = tmp;
                                     //   scanned++;
                                        break;

                                case 's':
                                        sp = va_arg(val, char *);
                       		//	printf("");
			                read_str(sp, 100);
                       		//	printf("After scanf\n");
				//	printf("In scanf val read: %s\n", sp);
                                       // scanned++;
                                        break;
                                case 'x':
                                        ip = va_arg(val, int *);
                                        *ip = read_hex();
                                       // scanned++;
                                        break;
				default:
					return 0;
					//return scanned;
                        }
                }
        ++format;
        }
//	printf("Before Return\n");
//	printf("\nBefore returning from scanf val read: %s\n", sp);
 //       return scanned;
        return 0;
}
