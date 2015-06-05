#include "sys/sbunix.h"
#include "sys/syscall.h"
#include <sys/gdt.h>

int ticks = 0;
void print_time(uint64_t time);


struct regs
{
     uint64_t r11, r10, r9, r8, rdi, rsi, rdx, rcx, rbx, rax; // Pushed by pushq i.e. all general purpose registers
     unsigned char error_code;
};

void divide_by_zero_handler(){
	printf("DIVIDE BY ZERO EXCEPTION!!! Killing the process now\n");
	exit_process();
	//while(1);
}

void timer_handler(struct regs_sched r)
{
    ticks++;
        outportb(0x20, 0x20);
    if (ticks % 18 == 0)
   {
//	print_time((ticks/18));
	decrement_alarm();
	schedule(r);
    }
}

void gpf_handler()
{
	printf("General Protection Fault\t");
	while(1);
	uint64_t inst_addr, error_code;
    __asm__ __volatile__ ("mov %%cr2, %0" : "=r" (inst_addr));
    __asm__ __volatile__ ("movq %%rsp, %0" : "=r" (error_code));
  //  printf("Faulting instruction and error code: %p %p\n", inst_addr, error_code);
}

void page_fault_handler(struct regs reg)
{
	
    uint64_t virt;
    __asm__ __volatile__ ("mov %%cr2, %0" : "=r" (virt));
//printf("----------HERE3 with error code %p %p\n", virt, reg.error_code);
    if((virt < VIRTUAL_OFFSET && current_process != NULL && !find_vma(current_process->mm_st->mmap, virt) && (reg.error_code & 0x4)) || (virt == 0x0)){
//	printf("Values: stack start, virt, limit %p %p %p\n", STACK_START, virt, (get_stack_end(current_process) - PAGE_SIZE));
	if(virt <= STACK_START && virt >= (get_stack_end(current_process) - PAGE_SIZE)){
//		printf("------------Virt address within a page of stack lower limit-------------\n");
		int ret = extend_stack(current_process, virt);
		if(ret == 0) {
			return;
		}
	}
//	printf("Segmentation Fault %p with error code : %p  !!! Killing the process Now\n", virt, reg.error_code);
//	print_vma(current_process);
//	while(1);
	printf("Segmentation Fault  !!! Killing the process Now\n");
	
	exit_process();
	return;
    }

 if(reg.error_code & 0x1){

	//READ and WRITE Violation May Be.
 	 uint64_t *pteAdd = (uint64_t *)(get_self_ref_add(0x1FE, get_pml4_offset(virt), get_pdp_offset(virt), get_pd_offset(virt)));
	if((reg.error_code & 0x2) &&  pteAdd[get_pt_offset(virt)] & COW_MASK){
		assign_page_cow(virt);
	}
	else{
		printf("Segmentation Fault!!! Killing the process Now\n");
		exit_process();
		return;
//		while(1);
    	}
    }
    else{
//	if(reg.error_code & 0x2) printf("WTF %p %p\n", virt, reg.error_code);
        //Page fault occured due to page not present
        assign_page_with_self_ref(virt);
    }
}

    /* Error code has following bits:
     Last(bit 0) : P: When set the fault was caused by page protection violation. Else page not present.
     Bit 1: W: When set fault was due to page write. Else page read.
     Bit 2: U: User: When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.
     Bit 3 : R : When set, the page fault was caused by reading a 1 in a reserved field. */


volatile uint64_t rip_read, rsp_read, rip_write, rsp_write;
volatile struct regs_sched r_global;
uint64_t syscall_handler_isha(struct regs_sched r) {
        uint64_t syscall_num = 0;
	uint64_t arg1, arg2, arg3, bytes_written, local_fd, global_fd;
	uint64_t ret = 0;
        uint64_t global_fd_tmp, local_fd_tmp;
	int fd0, fd1, i;
	int * fd;
	char * data;
	char *b;
	syscall_num = r.rax;
	arg1 = r.rdi;
	arg2 = r.rsi;
	arg3 = r.rdx;


    switch(syscall_num){
	case SYS_write:
		// arg1 : fd, arg2 : buf, arg3 : bytes
			rip_write = r.rip;
                rsp_write = r.rsp;
		r_global = r;
		if((int) arg1 < 0)
		{
			/* User given local fd can never be negative */
			ret = -9;
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return 0;
		}
		else
		{
			local_fd = get_direct_local_fd(current_process, (int)arg1);
			int tmp = current_process->file_descriptor[local_fd];

			if((local_fd == 1 && tmp == -1) || (local_fd == 2 && tmp == -1))
			{
				/* Write allowed to STDOUT or STDERR */

				b = (char *) arg2;
				bytes_written = 0;
				printf("");
				for (int i = 0; i < (int) arg3 && b[i] != '\0'; i++) {
					if (b[i] == '\n')
						printf("\n");
					else if (b[i] == '\t')
						printf("\t");
					else
						printf("%c", b[i]);
					bytes_written++;
				}
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(bytes_written));
				return bytes_written;
			}

			if(tmp == -88)
			{
				/* Write allowed in pipe buffer. */
				i = 0;
				data = (char *) arg2;

				//printf("******* Writing to circular buffer *******\n");
	            for (i = 0; i < (int) arg3 && data[i] != '\0'; i++)
				{
					ret = write_to_circular_buffer(data[i]);
					if(ret == 1)
					{
						continue;
					}
					else
					{
					/* Need to handle wait	*/
						break;
					}
				}
					__asm__ __volatile__("movq %0, 208(%%rsp)"::"r"(r_global.r15));
                    __asm__ __volatile__("movq %0, 216(%%rsp)"::"r"(r_global.r14));
                    __asm__ __volatile__("movq %0, 224(%%rsp)"::"r"(r_global.r13));
                    __asm__ __volatile__("movq %0, 232(%%rsp)"::"r"(r_global.r12));
                    __asm__ __volatile__("movq %0, 240(%%rsp)"::"r"(r_global.r11));
                    __asm__ __volatile__("movq %0, 248(%%rsp)"::"r"(r_global.r10));
                    __asm__ __volatile__("movq %0, 256(%%rsp)"::"r"(r_global.r9));
                    __asm__ __volatile__("movq %0, 264(%%rsp)"::"r"(r_global.r8));
                    __asm__ __volatile__("movq %0, 272(%%rsp)"::"r"(r_global.rbp));
                    __asm__ __volatile__("movq %0, 280(%%rsp)"::"r"(r_global.rdi));
                    __asm__ __volatile__("movq %0, 288(%%rsp)"::"r"(r_global.rsi));
                    __asm__ __volatile__("movq %0, 296(%%rsp)"::"r"(r_global.rdx));
                    __asm__ __volatile__("movq %0, 304(%%rsp)"::"r"(r_global.rcx));
                    __asm__ __volatile__("movq %0, 312(%%rsp)"::"r"(r_global.rbx));
                    __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)i));
                    __asm__ __volatile__("movq %0, 328(%%rsp)"::"r"(rip_write));
                    __asm__ __volatile__("movq %0, 336(%%rsp)"::"r"((uint64_t)0x1B));
                    __asm__ __volatile__("movq %0, 344(%%rsp)"::"r"((uint64_t)0x200286));
                    __asm__ __volatile__("movq %0, 352(%%rsp)"::"r"(rsp_write));
                    __asm__ __volatile__("movq %0, 360(%%rsp)"::"r"((uint64_t)0x23));
				return i;
			}
			else if(tmp == 1 || tmp == 2)
			{
				/* Write allowed to STDOUT or STDERR */

				b = (char *) arg2;
				bytes_written = 0;
				printf("");
				for (int i = 0; i < (int) arg3 && b[i] != '\0'; i++) {
					if (b[i] == '\n')
						printf("\n");
					else if (b[i] == '\t')
						printf("\t");
					else
						printf("%c", b[i]);
					bytes_written++;
				}
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(bytes_written));
				return bytes_written;
			}
			else
			{
//				printf("Write not allowed.\n");
				ret = -9;
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
				return ret;
			}
		}

	    case SYS_exit:
                exit_process();	   

 	    case SYS_getpid:
		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(current_process->pid));
		return current_process->pid;

	    case SYS_getppid:
		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(current_process->ppid));
		return current_process->ppid;

	    case SYS_brk:
		printf("");
		if(arg1 == 0){
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(current_process->brk_ptr));
			return current_process->brk_ptr;
		}
		ret = extend_heap(current_process, arg1);
		if(ret == 0){
    		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)-MYENOMEM));
	    	return -MYENOMEM;
		}
		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
		return ret;

	    case SYS_yield:
		printf("");
		schedule(r);
		return 0;

	    case SYS_fork:
            current_process->s_regs = r;
            ret = _fork(current_process);
//            __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(current_process->s_regs.rax));
            __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
            return ret;

	    case SYS_nanosleep:
            printf("");
            k_sleep(arg1,r);
            __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)0));
    		return 0;

        case SYS_wait4:
            printf("");
            ret = k_waitpid(arg1,r);
            __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)ret));
            return ret;

	    case SYS_execve:
            printf("");
            b = (char *)arg1;
//            printf("filename in timerc %s\n", b);
            ret = k_execve(b, (char **)arg2, (char **)arg3);
            __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)ret));
	    	return ret;
	   
	     case SYS_open:
		   /*
		    * Check if current process can access the file
		    * arg1 = filename
		    * arg2 = flags
		    */
		   //	printf("SYS_open, received: %s, %d\n",(char *)arg1, (int)arg2);
	    	global_fd = tarfs_open((char *) arg1, (int) arg2, current_process);

	    	if(global_fd == -13 || global_fd == -2)
	    	{
	    	//	printf("\nSYS_open: No entry named:%s, returning:%d\n",(char *)arg1, global_fd);
	    		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(global_fd));
	    		return 0;
	    	}

	    	ret = get_local_fd(current_process, global_fd);
	    	if(ret < 0)
	    	    ret = -24;
//	    	printf("SYS_open %s, returning: %d\n", (char *)arg1, ret);

	    	__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
	    	return ret;

	    case SYS_close:
	    	/*
	    	 * arg1 = local file descriptor
	    	 */
//	    	printf("SYS_close, received: %d\n", (int)arg1);
	    	local_fd = get_direct_local_fd(current_process, (int) arg1);

			if(local_fd < 0)
			{
//				printf("Cannot close. The local descriptor: %d is not mapped to anything!\n", (int) arg1);
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)-9));
				return 0;
			}

			global_fd = get_global_fd(current_process, local_fd);

	    	/* Trying to close pipe */
			if(current_process->file_descriptor[local_fd] == -77 || current_process->file_descriptor[local_fd] == -88)
			{
				reset_local_fd(current_process, (int) arg1);
				current_process->file_descriptor[(int)arg1] = -1;
				current_process->mapping[(int)arg1] = 0;
				current_process->f_pos[(int)arg1] = 0;
//				printf("SYS_close returning after %d\n", 0);
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)0));
				return 0;
			}

			reset_local_fd(current_process, (int) arg1);

	    	if(global_fd < 0)
	    	{
	    		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)-9));
	    		return -9;
	    	}

	    	ret = (uint64_t)tarfs_close(global_fd);

//	    	printf("SYS_close, returning: %d\n", ret);
	    	__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
	    	return ret;

	  case SYS_read:
                /*
                 * arg1 = local file descriptor
                 * arg2 = buffer to be filled
                 * arg3 = bytes to read
                 */
		rip_read = r.rip;
		rsp_read = r.rsp;
		r_global = r;
		  if((int) arg1 < 0)
		  {
			/* User given local fd can never be negative */
			ret = -9;
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return -9;
		  }

		  local_fd = get_direct_local_fd(current_process, (int) arg1);
		  int tmp = current_process->file_descriptor[local_fd];

		  if(tmp == -1)
		  {
			  if(local_fd == 0)
			  {
				  /* Read from STDIN */
//				printf("SyS Read in pid: %d and reading: %d\n", current_process->pid, reading);
				  if(reading != 0){
				  	if(current_process->pid != fg_process->pid)
				  	{
						  exit_process();
						  __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)(-1 * MYEACCES)));
						  return -MYEACCES;
				  	}
					else{
						kill_process(reading);
					}
				    }

				reading = current_process->pid;
				b = (char *)arg2;
				read_from_stdin(b, arg3);
				strncpy(b, (char *)global_buf, arg3);
				if(arg3>1)
					b[global_buf_index] = '\0';
				ret = global_buf_index;
				memset((char *)global_buf, 0, BUF_SIZE);
				global_buf_index = 0;

				__asm__ __volatile__("movq %0, 208(%%rsp)"::"r"(r_global.r15));
				__asm__ __volatile__("movq %0, 216(%%rsp)"::"r"(r_global.r14));
				__asm__ __volatile__("movq %0, 224(%%rsp)"::"r"(r_global.r13));
				__asm__ __volatile__("movq %0, 232(%%rsp)"::"r"(r_global.r12));
				__asm__ __volatile__("movq %0, 240(%%rsp)"::"r"(r_global.r11));
				__asm__ __volatile__("movq %0, 248(%%rsp)"::"r"(r_global.r10));
				__asm__ __volatile__("movq %0, 256(%%rsp)"::"r"(r_global.r9));
				__asm__ __volatile__("movq %0, 264(%%rsp)"::"r"(r_global.r8));
				__asm__ __volatile__("movq %0, 272(%%rsp)"::"r"(r_global.rbp));
				__asm__ __volatile__("movq %0, 280(%%rsp)"::"r"(r_global.rdi));
				__asm__ __volatile__("movq %0, 288(%%rsp)"::"r"(r_global.rsi));
				__asm__ __volatile__("movq %0, 296(%%rsp)"::"r"(r_global.rdx));
				__asm__ __volatile__("movq %0, 304(%%rsp)"::"r"(r_global.rcx));
				__asm__ __volatile__("movq %0, 312(%%rsp)"::"r"(r_global.rbx));
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
				__asm__ __volatile__("movq %0, 328(%%rsp)"::"r"(rip_read));
				__asm__ __volatile__("movq %0, 336(%%rsp)"::"r"((uint64_t)0x1B));
				__asm__ __volatile__("movq %0, 344(%%rsp)"::"r"((uint64_t)0x200286));
				__asm__ __volatile__("movq %0, 352(%%rsp)"::"r"(rsp_read));
				__asm__ __volatile__("movq %0, 360(%%rsp)"::"r"((uint64_t)0x23));

				reading = 0;				
				return ret;
			  }
			  else
			  {
				  /* Dont allow read */
				  ret = -9;
				  __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
				  return ret;
			  }
		  }
		  if(tmp == -88 || tmp == 1 || tmp == 2)
		  {
			  /* Read not allowed. */
			  ret = -9;
			  __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			  return ret;
		  }
		  else if(tmp == -77)
		  {
//			  printf("******* Reading from circular buffer\n %s*******\n", circular_buffer);
			  b = (char *)arg2;
			  ret = read_from_circular_buffer(b, (int) arg3);
			  __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			  return ret;
		  }
		  else if(tmp == 0)
		  {
//			  printf("You are trying to read from STDIN\n");

			  if(current_process->pid != fg_process->pid)
			  {
					  exit_process();
					  __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)(-1 * MYEACCES)));
					   return -MYEACCES;
			  }

				b = (char *)arg2;
				read_from_stdin(b, arg3);
				strncpy(b, (char *)global_buf, arg3);
				if(arg3>1)
					b[global_buf_index] = '\0';
				ret = global_buf_index;
				memset((char *)global_buf, 0, BUF_SIZE);
				global_buf_index = 0;
 				__asm__ __volatile__("movq %0, 208(%%rsp)"::"r"(r_global.r15));
                                __asm__ __volatile__("movq %0, 216(%%rsp)"::"r"(r_global.r14));
                                __asm__ __volatile__("movq %0, 224(%%rsp)"::"r"(r_global.r13));
                                __asm__ __volatile__("movq %0, 232(%%rsp)"::"r"(r_global.r12));
                                __asm__ __volatile__("movq %0, 240(%%rsp)"::"r"(r_global.r11));
                                __asm__ __volatile__("movq %0, 248(%%rsp)"::"r"(r_global.r10));
                                __asm__ __volatile__("movq %0, 256(%%rsp)"::"r"(r_global.r9));
                                __asm__ __volatile__("movq %0, 264(%%rsp)"::"r"(r_global.r8));
                                __asm__ __volatile__("movq %0, 272(%%rsp)"::"r"(r_global.rbp));
                                __asm__ __volatile__("movq %0, 280(%%rsp)"::"r"(r_global.rdi));
                                __asm__ __volatile__("movq %0, 288(%%rsp)"::"r"(r_global.rsi));
                                __asm__ __volatile__("movq %0, 296(%%rsp)"::"r"(r_global.rdx));
                                __asm__ __volatile__("movq %0, 304(%%rsp)"::"r"(r_global.rcx));
                                __asm__ __volatile__("movq %0, 312(%%rsp)"::"r"(r_global.rbx));
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
				__asm__ __volatile__("movq %0, 328(%%rsp)"::"r"(rip_read));
				__asm__ __volatile__("movq %0, 336(%%rsp)"::"r"((uint64_t)0x1B));
				__asm__ __volatile__("movq %0, 344(%%rsp)"::"r"((uint64_t)0x200286));
				__asm__ __volatile__("movq %0, 352(%%rsp)"::"r"(rsp_read));
				__asm__ __volatile__("movq %0, 360(%%rsp)"::"r"((uint64_t)0x23));
				return ret;
		  }
		  else
		  {
			  /* Reading from any file */
			  global_fd = get_global_fd(current_process, local_fd);

			  if (global_fd < 0)
			  {
		//		printf("SYS_read, returning %d\n", global_fd);
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(global_fd));
				return global_fd;
			  }

			  ret = (uint64_t) tarfs_read_file(global_fd, (char *) arg2, (int) arg3,
					  current_process, local_fd);

		//	  printf("SYS_read, returning %d\n", (int) ret);
			  __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			  return ret;
		  }

	    case SYS_lseek:
			/*
			 * arg1 = local file des
			 * arg2 = offset to move
			 * arg3 whence
			 */
//        	printf("SYS_lseek, received: %d, %d, %d\n", (int)arg1, (int)arg2, (int) arg3);

        	local_fd = get_direct_local_fd(current_process, (int) arg1);

        	if(local_fd < 0)
			{
				ret = -9;
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
				return ret;
			}

			global_fd = get_global_fd(current_process, (int) arg1);

			if(global_fd <= 0)
			{
				__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(global_fd));
				return global_fd;
			}

			ret = tarfs_lseek(global_fd, current_process, local_fd, (int) arg2, (int)arg3);
//			printf("SYS_lseek returning: %d\n", ret);
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return ret;

	    case SYS_getcwd:
			ret = copy_current_directory(current_process, (char *)arg1, (int)arg2);

			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return ret;

	    case SYS_chdir:
		if(!arg1){
			ret = -MYENOTDIR;
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return ret;
		}
	    	ret = tarfs_change_directory(current_process, (char *)arg1);
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return ret;

        case SYS_kill:
//            printf("Killing %d\n", arg1);
            ret = kill_process(arg1);
            __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
            return ret;
            
        case SYS_ps:
//            printf("Inside ps\n");
            ret = print_processes();
            __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
            return ret;
            
	    case SYS_dup:
//	    	printf("SYS_dup received: %d\n", (int) arg1);

	    	local_fd = get_direct_local_fd(current_process, (int) arg1);

	    	if(local_fd <= 0)
	    	{
	    		ret = -9;
//	    		printf("SYS_dup returning -9\n");
	    		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
	    		return ret;
	    	}

	    	ret = duplicate_fd(current_process, local_fd);
//	    	printf("SYS_dup  returning: %d\n", ret);

	    	__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
	    	return ret;

	    case SYS_dup2:
                /*
                 * arg1 : old local fd
                 * arg1 : new local  fd
                 */
                //printf("SYS_dup2 received: %d, %d\n", (int) arg1, (int) arg2);

                if((int) arg1 < 0 || (int) arg2 < 0)
                {
                	/*  Local fd can never be less than zero */
                	__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)-9));
                	return -9;
                }

                local_fd = get_direct_local_fd(current_process, (int) arg1);

                if (local_fd < 0) {
                        ret = -9;
//                        printf("SYS_dup2 returning -9\n");
                        __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
                        return ret;
                }

                /*  Check if new fd is already valid and open */
                local_fd_tmp = get_direct_local_fd(current_process, (int) arg2);
                if (local_fd_tmp >= 0) {
                    	reset_local_fd(current_process, (int) arg2);
                        global_fd_tmp = get_global_fd(current_process, local_fd_tmp);

                        if (global_fd_tmp < 0) {
                                // nothing to do, just go ahead with duplication process
                        } else {
                                tarfs_close(global_fd_tmp);
                        }
                }

                // Now, new fd is not valid, so go ahead with duplication process

                ret = duplicate_fd_2(current_process, local_fd, (int) arg2);
               // printf("SYS_dup2  returning: %d\n", ret);

    	    	global_fd = get_global_fd(current_process, local_fd);
	    	    inc(global_fd);

                __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
                return ret;

	  case SYS_alarm:
                if(arg1<=0)
                {
                        ret= current_process->alarm_time;
                        __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
                        return ret;
                }
                else
                {
                        ret= current_process->alarm_time;
                        current_process->alarm_time = arg1;
                        __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
                        return ret;

                }

	case SYS_pipe:
		/*
		 * arg1 = fd[2] array
		 *
		 * get 2 free descriptors for current process and map them to stdin and stdout
		 * and return them.
		 *
		 * NOTE: -77 is pipe read, -88 is pipe write
		 */

		/* Read end of pipe */

		fd0 = get_free_fd(current_process);
		if(fd0 == 0 ){
		    ret = -24;
		    __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
      		    return ret;

		}
		current_process->file_descriptor[fd0] = -77;
		current_process->mapping[fd0] = 1;
		current_process->f_pos[fd0] = 0;

		/* Write end of pipe */

		fd1 = get_free_fd(current_process);
		if(fd1 == 0 ){
		    ret = -24;
		    __asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
        	    return ret;

		}
		current_process->file_descriptor[fd1] = -88;
		current_process->mapping[fd1] = 1;
		current_process->f_pos[fd1] = 0;

		fd = (int *) arg1;
		*fd = fd0;
		fd++;
		*fd = fd1;
		//memset((char *)circular_buffer, 0, CIRCULAR_BUF_SIZE);
		//read_ptr = 0;
		//write_ptr = 0;

		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
		return ret;

	case SYS_getdents:
		/*
		 * arg1 = fd of directory
		 * arg2 = buffer to be filled.
		 * arg3 = size of buffer
		 */

		if((int) arg1 < 0)
		{
			/*  Local fd can never be less than zero */
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"((uint64_t)-9));
			return -9;
		}

		local_fd = get_direct_local_fd(current_process, (int)arg1);

		if(local_fd < 0)
		{
			ret = -9;
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return ret;
		}

		global_fd = get_global_fd(current_process, local_fd);

		if(global_fd < 0)
		{
			ret = -9;
			__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
			return ret;
		}

		ret = tarfs_read_dir(global_fd, (char *)arg2,  arg3, current_process, local_fd);

		__asm__ __volatile__("movq %0, 320(%%rsp)"::"r"(ret));
        return ret;

	}

        return 0;
}
