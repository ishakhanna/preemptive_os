#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string_helper.h"

int print_str(char *str){
    int printed = 0;
    while(*str){
        write(1,str,1);
        ++str;
        ++printed;
    }
    return printed;
}

int print_num(int a){
    if(a == 0){
	return print_str("0");
    }
    char *num_str = itoa(a);
    return print_str(num_str);
}

int print_hex(int a){
    char *num_str = itohex(a);
    return print_str(num_str);
}

int print_add(uint64_t a){
    if (a == 0) {
	return print_str("0x0");
    }

    char *num_str = ultohex(a);
    return print_str(num_str);
}

int printf(const char *format, ...) {
	va_list val;
	int printed = 0;

	va_start(val, format);
	while(*format) {
		if(*format == '%'){
			++format;
			if(*format == 'd'){
				int a = va_arg(val, uint64_t);
		                int printed_bytes = print_num(a);
				++format;
                		printed += printed_bytes;
			}
            		else if(*format == 'c'){
                		char a = (char) va_arg(val, int);
                 		write(1,&a,1);
                		++format;
                		printed++;
            		}
            		else if(*format == 's'){
                		char *a = va_arg(val, char*);
                		int printed_bytes = printf(a);
				++format;
        		        printed += printed_bytes;
            		}
			else if(*format == 'x'){
				int a = va_arg(val, uint64_t);
                                int printed_bytes = print_hex(a);
                                ++format;
                                printed += printed_bytes;
			}
			else if(*format == 'p'){
                                uint64_t a = va_arg(val, uint64_t);
                                print_add(a);
                                ++format;
                        }
		}
		else{
			write(1, format, 1);
			++printed;
			++format;
		}
	}
	return printed;
}
