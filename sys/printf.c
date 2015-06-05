#include <stdarg.h>
#include "sys/sbunix.h"

int colour = 0;
volatile char *video = (volatile char*)(0xB8000);
volatile char * VIDEO_ADDR = (volatile char *)(0xB8000);
void write_str_on_console(const char *str){
        int i = 0;
        while( str[i] != 0 )
        {
                *video++= str[i];
                *video++ = colour;
                i++;
        }
}

void write_on_console(char str){
                *video++= str;
                *video++ = colour;
}

void print_num(int a){
    if (a == 0){
	write_on_console('0');
	return;
    }
    char *num_str = itoa(a);
    write_str_on_console(num_str);
}

void print_hex(int a){
   if (a == 0){
        write_on_console('0');
        return;
    } 
    if(a > 0){
    	char *num_str = itohex(a); 
	write_str_on_console(num_str);
    }
}

void print_add(uint64_t a){
    if (a == 0) {
	write_str_on_console("0x0");
	return;
    }

    char *num_str = ultohex(a); 
    write_str_on_console(num_str);
}

void print_time(uint64_t time){
        uint64_t i = 0;
	char *str = ltoa(time);
        volatile char *vi = (volatile char*)(VIDEO_ADDR);
        vi += (25*158);
        while(str[i] != 0)
        {
                *vi++= str[i];
                *vi++ = colour;
                i++;
        }
}

void print_backspace()
{
	--video;
	--video;

	*video = ' ';
	*video = colour;
}

void print_glyph(char ch)
{
/* Move to bottom left cormer */
	volatile char *vi = (volatile char*)(VIDEO_ADDR);
        vi += (25*158);

	vi = vi - 110;
/* Clear out previously written characters */	
	*vi++ = ' ';
	*vi++ = colour;

	*vi++ = ' ';
	*vi++ = colour;

	*vi--;
	*vi--;
	*vi--;
	*vi--;

	*vi++ = ch;
	*vi++ = colour;	
}

void print_glyphs(char *ch)
{
/* Move to bottom left corner */
        volatile char *vi = (volatile char*)(VIDEO_ADDR);
        vi += (25*158);
	vi = vi - 110;

/* Delete previously written char */
        *vi++ = ' ';
        *vi++ = colour;
        *vi++ = ' ';
        *vi++ = colour;

/* Come back to start position */
        *vi--;
        *vi--;
        *vi--;
        *vi--;
/* Print new characters */
        *vi++ = ch[0];
	*vi++ = colour;
	*vi++ = ch[1];
	*vi++ = colour;    	
}

void printf(const char *format, ...) {
	va_list val;
	va_start(val, format);
	int64_t trav;
	while(*format) {
		trav = (int64_t)(video - (VIDEO_ADDR));
                if(trav == (160*24)){
                        volatile char * i = (volatile char*)(VIDEO_ADDR);
                        for(;i<(video); i += 160){
                                memcpy((void *)i, (void *)(i+160), 160);
                        }
                        video -= 160;
                }

		if(*format == '%'){
			++format;
			if(*format == 'c'){
                                char a = (char) va_arg(val, int);
                                write_on_console(a);
                                ++format;
                        }
			else if(*format == 'd'){
				int a = va_arg(val, uint64_t);
		                print_num(a);
				++format;
			}
            		else if(*format == 's'){
                		char *a = va_arg(val, char*);
                		printf(a);
				++format;
            		}
			else if(*format == 'x'){
				int a = va_arg(val, uint64_t);
                                print_hex(a);
                                ++format;
			}
			else if(*format == 'p'){
				uint64_t a = va_arg(val, uint64_t);
				print_add(a);
				++format;
			}
		}
		else if(*format == '\n'){
			format++;
			uint64_t offset = trav % 160;
			video += (160-offset);
		}
		else if(*format == '\t'){
			video += 10;
			format++;
		}
		else{
			write_on_console(*format);
			++format;
		}
	}
}
