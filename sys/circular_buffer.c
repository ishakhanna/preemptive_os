#include <sys/sbunix.h>

int increment(int ptr)
{
	if(ptr + 1 >= CIRCULAR_BUF_SIZE)
	{
		ptr = 0;
	}
	else
	{
		++ptr;
	}
	return ptr;
}

// if difference between write and read = BUFF MAX, then stop writing
// if difference between write and read is 0, then stop reading

int difference()
{
	if(write_ptr > read_ptr)
	{
		return write_ptr - read_ptr;
	}
	else
	{
		return CIRCULAR_BUF_SIZE -(read_ptr - write_ptr);
	}
}


int write_to_circular_buffer(char c)
{
	int bytes_avail_to_read = difference();
//	printf("Bytes to read %d\t", bytes_avail_to_read);
	if(bytes_avail_to_read == (CIRCULAR_BUF_SIZE-1)) 
	{
		/* Buffer is full. stop writing */
		has_bytes_to_write = 0;
		__asm__ __volatile__("sti");
		while(1){
			if(has_bytes_to_write == 1)
				break;
		}
	}
       circular_buffer[write_ptr] = c;
        write_ptr = increment(write_ptr);
        return 1;
}

char read_a_char_from_circular_buffer()
{
	char c = circular_buffer[read_ptr];
	circular_buffer[read_ptr] = 0;
	read_ptr = increment(read_ptr);
	has_bytes_to_write = 1;
	return c;
}

int read_from_circular_buffer(char * buf, int bytes_to_read)
{
	int bytes_avail_to_read = difference();
	int i = 0;
	while(bytes_avail_to_read > 0 && bytes_to_read > 0)
	{
		buf[i] = read_a_char_from_circular_buffer();
		bytes_avail_to_read--;
		bytes_to_read--;
		i++;
	}
	
	if(i > 1)
		buf[i] = '\0';

	return i;
}
