#include <sys/sbunix.h>
#define ON 1
#define OFF 0

void keyboard_handler();
char get_ascii(uint64_t code);
extern void print_glyphs(char *c);
extern void print_glyph(char);

int shift_key, ctrl_key, caps_key;
char string[2] = {' ', ' '};
char ch;

enum KEYBOARD_CONTROLLER
{
	KEYBOARD_DATA_REGISTER = 0x60,
	KEYBOARD_CONTROL_REGISTER = 0x64 // also called Status register to know keyboard status.
};

typedef enum
{
  KEY_SPACE             = ' ',
  KEY_0                 = '0',
  KEY_1                 = '1',
  KEY_2                 = '2',
  KEY_3                 = '3',
  KEY_4                 = '4',
  KEY_5                 = '5',
  KEY_6                 = '6',
  KEY_7                 = '7',
  KEY_8                 = '8',
  KEY_9                 = '9',
  KEY_A                 = 'a',
  KEY_B                 = 'b',
  KEY_C                 = 'c',
  KEY_D                 = 'd',
  KEY_E                 = 'e',
  KEY_F                 = 'f',
  KEY_G                 = 'g',
  KEY_H                 = 'h',
  KEY_I                 = 'i',
  KEY_J                 = 'j',
  KEY_K                 = 'k',
  KEY_L                 = 'l',
  KEY_M                 = 'm',
  KEY_N                 = 'n',
  KEY_O                 = 'o',
  KEY_P                 = 'p',
  KEY_Q                 = 'q',
  KEY_R                 = 'r',
  KEY_S                 = 's',
  KEY_T                 = 't',
  KEY_U                 = 'u',
  KEY_V                 = 'v',
  KEY_W                 = 'w',
  KEY_X                 = 'x',
  KEY_Y                 = 'y',
  KEY_Z                 = 'z',
  KEY_RETURN            = '\r',
  KEY_ESCAPE            = 0x1001,
  KEY_BACKSPACE         = '\b',
  KEY_UP                = 0x1100,
  KEY_DOWN              = 0x1101,
  KEY_LEFT              = 0x1102,
  KEY_RIGHT             = 0x1103,
  KEY_F1                = 0x1201,
  KEY_F2                = 0x1202,
  KEY_F3                = 0x1203,
  KEY_F4                = 0x1204,
  KEY_F5                = 0x1205,
  KEY_F6                = 0x1206,
  KEY_F7                = 0x1207,
  KEY_F8                = 0x1208,
  KEY_F9                = 0x1209,
  KEY_F10               = 0x120a,
  KEY_F11               = 0x120b,
  KEY_F12               = 0x120b,
  KEY_F13               = 0x120c,
  KEY_F14               = 0x120d,
  KEY_F15               = 0x120e,
  KEY_DOT               = '.',
  KEY_COMMA             = ',',
  KEY_COLON             = ':',
  KEY_SEMICOLON         = ';',
  KEY_SLASH             = '/',
  KEY_BACKSLASH         = '\\',
  KEY_PLUS              = '+',
  KEY_MINUS             = '-',
  KEY_ASTERISK          = '*',
  KEY_EXCLAMATION       = '!',
  KEY_QUESTION          = '?',
  KEY_QUOTEDOUBLE       = '\"',
  KEY_QUOTE             = '\'',
  KEY_EQUAL             = '=',
  KEY_HASH              = '#',
  KEY_PERCENT           = '%',
  KEY_AMPERSAND         = '&',
  KEY_UNDERSCORE        = '_',
  KEY_LEFTPARENTHESIS   = '(',
  KEY_RIGHTPARENTHESIS  = ')',
  KEY_LEFTBRACKET       = '[',
  KEY_RIGHTBRACKET      = ']',
  KEY_LEFTCURL          = '{',
  KEY_RIGHTCURL         = '}',
  KEY_DOLLAR            = '$',
  KEY_EURO              = '$',
  KEY_LESS              = '<',
  KEY_GREATER           = '>',
  KEY_BAR               = '|',
  KEY_GRAVE             = '`',
  KEY_TILDE             = '~',
  KEY_AT                = '@',
  KEY_CARRET            = '^',
  KEY_KP_0              = '0',
  KEY_KP_1              = '1',
  KEY_KP_2              = '2',
  KEY_KP_3              = '3',
  KEY_KP_4              = '4',
  KEY_KP_5              = '5',
  KEY_KP_6              = '6',
  KEY_KP_7              = '7',
  KEY_KP_8              = '8',
  KEY_KP_9              = '9',
  KEY_KP_PLUS           = '+',
  KEY_KP_MINUS          = '-',
  KEY_KP_DECIMAL        = '.',
  KEY_KP_DIVIDE         = '/',
  KEY_KP_ASTERISK       = '*',
  KEY_KP_NUMLOCK        = 0x300f,
  KEY_KP_ENTER          = 0x3010,
  KEY_TAB               = 0x4000,
  KEY_CAPSLOCK          = 0x4001,
  KEY_LSHIFT            = 0x4002,
  KEY_LCTRL             = 0x4003,
  KEY_LALT              = 0x4004,
  KEY_LWIN              = 0x4005,
  KEY_RSHIFT            = 0x4006,
  KEY_RCTRL             = 0x4007,
  KEY_RALT              = 0x4008,
  KEY_RWIN              = 0x4009,
  KEY_INSERT            = 0x400a,
  KEY_DELETE            = 0x400b,
  KEY_HOME              = 0x400c,
  KEY_END               = 0x400d,
  KEY_PAGEUP            = 0x400e,
  KEY_PAGEDOWN          = 0x400f,
  KEY_SCROLLLOCK        = 0x4010,
  KEY_PAUSE             = 0x4011,
  KEY_UNKNOWN,
  KEY_NUMKEYCODES
} ASCII;

static int keyboard_code[] = 
{
  KEY_UNKNOWN,
  KEY_ESCAPE,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_0,
  KEY_MINUS,
  KEY_EQUAL,
  KEY_BACKSPACE,
  KEY_TAB,
  KEY_Q,
  KEY_W,
  KEY_E,
  KEY_R,
  KEY_T,
  KEY_Y,
  KEY_U,
  KEY_I,
  KEY_O,
  KEY_P,
  KEY_LEFTBRACKET,
  KEY_RIGHTBRACKET,
  KEY_RETURN,
  KEY_LCTRL,
  KEY_A,
  KEY_S,
  KEY_D,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_SEMICOLON,
  KEY_QUOTE,
  KEY_GRAVE,
  KEY_LSHIFT,
  KEY_BACKSLASH,
  KEY_Z,
  KEY_X,
  KEY_C,
  KEY_V,
  KEY_B,
  KEY_N,
  KEY_M,
  KEY_COMMA,
  KEY_DOT,
  KEY_SLASH,
  KEY_RSHIFT,
  KEY_KP_ASTERISK,
  KEY_RALT,
  KEY_SPACE,
  KEY_CAPSLOCK,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_KP_NUMLOCK,
  KEY_SCROLLLOCK,
  KEY_HOME,
  KEY_KP_8,
  KEY_PAGEUP,
  KEY_KP_2,
  KEY_KP_3,
  KEY_KP_0,
  KEY_KP_DECIMAL,
  KEY_UNKNOWN,
  KEY_UNKNOWN,
  KEY_UNKNOWN,
  KEY_F11,
  KEY_F12
};

/* Function to get keyboard status */
uint64_t keyboard_status()
{
	return inportb(KEYBOARD_CONTROL_REGISTER);
}

/* This function MUST be called before doing any operations. */
int keyboard_ready()
{
	while(1)
	{
		if((keyboard_status() & 2) == 0) // 2 is mask for 0010 to get second bit
		{
			return 1;
		}
		// loop otherwise until keyboard is ready.
	}
}

/* Function to read scancode from keyboard */
uint64_t read_keyboard()
{
	return inportb(KEYBOARD_DATA_REGISTER);
}

void keyboard_handler()
{
	uint64_t scancode;
	ASCII key;

	if(keyboard_ready())
	{
		scancode = read_keyboard();

		if(scancode & 0x80) // break code
		{
			/* Set shift to OFF */
			scancode -= 0x80;
			key = keyboard_code[scancode];
			switch(key)
			{
			  case KEY_LSHIFT:
			  case KEY_RSHIFT:
				  shift_key = OFF;
				  break;
			  case KEY_LCTRL:
			  case KEY_RCTRL:
				 ctrl_key = OFF;
				 break;
			  case KEY_CAPSLOCK:
				 caps_key = OFF;
				 break;
			  default:
				  break;
			}

		}
		else
		{ 	//otherwise make code or normal character
			key = keyboard_code[scancode];

			/* Set shift/caps to ON */
			switch(key)
			{
		          case KEY_LSHIFT:
		          case KEY_RSHIFT:
		                  shift_key = ON;
		                  break;
			  case KEY_LCTRL:
			  case KEY_RCTRL:
				  ctrl_key = ON;
				  break;
			  case KEY_CAPSLOCK:
				  caps_key = ON;
				  break;
	                  default:
				/* Otherwise normal character, so print */
				if(scancode == 0xe)
				{
					//print_glyphs("\\b");
					//printf("\\b");
					print_backspace();
					decrement_buffer_index();
					//write_to_buffer('\b');
					break;
				}
				else if(scancode == 0x1c)
				{
					//print_glyphs("^M");
				//	printf("\n");
				//	write_to_buffer('\n');
					write_to_buffer('\0');
					newline = 1;
					//printf("\n User Entered: %s\n", global_buf);
					//printf("^M");
					break;
				}
				else if(scancode == 0xf)
				{
					//print_glyphs("^I");
					print_glyphs("^I");
					write_str_to_buffer("\\I");
					break;
				}
				/* Printing space for arrow keys */
				else if(scancode == 0x4b || scancode == 0x4d ||
					scancode == 0x48 || scancode == 0x50)
				{
					//print_glyphs("  ");
					//printf("");
					// dont print anything for arrow.
					break;
				}

				ch = get_ascii(key);
				if(ctrl_key == ON)
				{
					string[0] = '^';
					string[1] = ch;
					write_str_to_buffer(string);
					printf("%s",string);
					//print_glyphs(string);
				}
				else
				{
					//print_glyph(get_ascii(key));
					write_to_buffer(get_ascii(key));
					printf("%c",get_ascii(key));
				}
				break;
			}
		}		
	}
		outportb(0x20, 0x20);
}

char get_ascii(uint64_t k)
{
    ASCII key = k;

    if(shift_key == ON)
    {
        if(key >= 'a' && key <= 'z')
            key -= 32;
        else if(key >= '0' && key <= '9'){
            switch(key){
                case '0':
                      key = KEY_RIGHTPARENTHESIS;
                      break;
                case '1':
                      key = KEY_EXCLAMATION;
                      break;
                case '2':
                      key = KEY_AT;
                      break;
                case '3':
                      key = KEY_HASH;
                      break;
                case '4':
                      key = KEY_DOLLAR;
                      break;
                case '5':
                      key = KEY_PERCENT;
                      break;
                case '6':
                      key = KEY_CARRET;
                      break;
                case '7':
                      key = KEY_AMPERSAND;
                      break;
                case '8':
                      key = KEY_ASTERISK;
                      break;
                case '9':
                      key = KEY_LEFTPARENTHESIS;
                      break;
                default:
                      break;
            }
        }
        else{
            switch(key){
                case KEY_COMMA:
                      key = KEY_LESS;
                      break;
                case KEY_DOT:
                      key = KEY_GREATER;
                      break;
                case KEY_SLASH:
                      key = KEY_QUESTION;
                      break;
                case KEY_SEMICOLON:
                      key = KEY_COLON;
                      break;
                case KEY_QUOTE:
                      key = KEY_QUOTEDOUBLE;
                      break;
                case KEY_LEFTBRACKET:
                      key = KEY_LEFTCURL;
                      break;
                case KEY_RIGHTBRACKET:
                      key = KEY_RIGHTCURL;
                      break;
                case KEY_MINUS:
                      key = KEY_UNDERSCORE;
                      break;
                case KEY_EQUAL:
                      key = KEY_PLUS;
                      break;
                case KEY_BACKSLASH:
                      key = KEY_BAR;
                      break;
                case KEY_GRAVE:
                      key = KEY_TILDE;
                      break;
                default:
                      break;  
            }
        }
    }
    return key;
}
