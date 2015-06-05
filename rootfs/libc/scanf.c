#include "stdarg.h"
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

int read_int()
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
                        else
                        {
                                printf("Not a decimal\n");
                                break;
                        }
                }
        }

        return value;
}

char *read_str(char *str)
{
        char c;
        int i = 0;

        while(1)
        {
                c = read_char();
                if(c == ' ' || c == '\n')
                {
                        str[i] = '\0';
                        break;
                }
                else
                {
                        str[i] = c;
                        i++;
                }
        }
        return str;
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
        int scanned = 0;

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
                                        scanned++;
                                        break;
                                case 'c':
                                        cp = va_arg(val, char *);
                                        *cp = read_char();
                                        scanned++;
                                        break;
                                case 's':
                                        sp = va_arg(val, char *);
                                        sp = read_str(sp);
                                        scanned++;
                                        break;
                                case 'x':
                                        ip = va_arg(val, int *);
                                        *ip = read_hex();
                                        scanned++;
                                        break;
                                default:
                                        return scanned;
                        }
                }
        ++format;
        }
        return scanned;
}
