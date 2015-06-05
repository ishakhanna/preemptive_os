#include <sys/sbunix.h>

void write_to_buffer(char c){
	if(global_buf_index < BUF_SIZE){
		global_buf[global_buf_index++] = c;
	}
	else{
		global_buf[global_buf_index-1] = c;
	}	
}

void decrement_buffer_index()
{
	if(global_buf_index == 0)
		return;
	--global_buf_index;
}

void write_str_to_buffer(char *c){
	while(*c != '\0'){
		if(global_buf_index < BUF_SIZE){
			global_buf[global_buf_index++] = c[0];
		}
		else{
			global_buf[global_buf_index-1] = c[0];
		}
		c++;	
	}
}
void read_from_stdin(char *buffer, uint64_t bytes){
//	printf("\nINSIDE READ STDIN FUNCTION\n");

	__asm__ __volatile__("sti");
	while(1){
		 //if(newline == 1 && fg_process && fg_process->pid == current_process->pid)
		 if(newline == 1)
			break;
	}
//	printf("NEW LINE ENTERED in pid: %d\n",current_process->pid);
	newline = 0;
	return;
}
