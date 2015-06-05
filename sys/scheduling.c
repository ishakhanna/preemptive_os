#include "sys/sbunix.h"
#include <sys/gdt.h>
//#include "timer.c"

struct pcb_list *ready_queue = NULL;
struct pcb_list *wait_queue = NULL;
struct pcb_list *terminated_queue = NULL;
extern int ticks;
extern uint64_t *table_base_ptr;

void print_list(struct pcb_list **list) {
	struct pcb_list *curr = *list;
	while(curr != NULL){
		printf("pid: %d   ", curr->task->pid);
		curr = curr->next;
	}
	printf("\n");
}

void decrement_alarm()
{

// decrease alarm time for wait queue
  struct pcb_list *list = wait_queue;
        while (list != NULL) {
                if (list->task->alarm_time > 0)
                        list->task->alarm_time--;
                list = list->next;
        }

// decrease alarm time for ready queue
  list = ready_queue;
         while (list != NULL) {
                if (list->task->alarm_time > 0)
                        list->task->alarm_time--;
                list = list->next;
        }
}


void check_sleeping_procs() {
	struct pcb_list *list = wait_queue;
	while (list != NULL) {
		if (list->task->sleep_time > 0)
			list->task->sleep_time--;
		if (list->task->sleep_time <= 0 && list->task->is_waiting_for == 0) {
			insert_to_tail_pcb_list(&ready_queue, list->task);
			delete_in_pcb_list(&wait_queue, list->task);
		}
		list = list->next;
	}
}

//void move_to_tail(struct pcb_list** list) {
int move_to_tail(struct pcb_list** list) {
	if (*list == NULL) {
//		printf("List is empty\n");
		//while(1);
		return -1;
	}

	//just one pcb in list
	if ((*list)->next == NULL) {
//		printf("just one pcb in list");
		return 0;
	}
	struct pcb_list *temp1;
	struct pcb_list *temp2;
	temp1 = temp2 = *list;
	*list = (*list)->next;

	while (temp1->next != NULL)
		temp1 = temp1->next;
	temp1->next = temp2;
	temp2->next = NULL;
	return 0;
}

void delete_in_pcb_list(struct pcb_list **list, pcb *proc) {
	// Empty list
	if (*list == NULL) {
	    return;
//		printf("Error.. No Process.. !!\n");
//		while (1);
	}

	// When pcb to be deleted is first pcb
	if ((*list)->task->pid == proc->pid) {
		*list = (*list)->next;
		//TODO nitish free the node
	} else {
		struct pcb_list *temp = *list;
		while (temp->next->task != proc && temp->next != NULL)
			temp = temp->next;
		if (temp->next->task == proc) {
			temp->next = temp->next->next;
		}
	}
}

void insert_to_tail_pcb_list(struct pcb_list **list, pcb *proc) {
        struct pcb_list *temp = (struct pcb_list *) kmalloc1(
                        sizeof(struct pcb_list));
  	struct pcb_list *iter = *list; 
	temp->task = proc;
        temp->next = NULL;
        if (*list == NULL) {
                *list = temp;
        } else {
                while (iter->next != NULL)
                        iter = iter->next;
                iter->next = temp;
        }
}

void insert_to_head_pcb_list(struct pcb_list **list, pcb *proc) {
	struct pcb_list *temp = (struct pcb_list *) kmalloc1(
			sizeof(struct pcb_list));
	temp->task = proc;
	temp->next = *list;
	//if(*list!=NULL)
	*list = temp;
}
void context_switch(){
    pcb *next_process;
    if(ready_queue == NULL){
//                printf("Schedule: Next process null. So scheduling an idle process\n");
		next_process = idle_process;
     }
    else{
	next_process = ready_queue->task;
    }
//printf("NEXT process [pid: %d and rip %p\n", next_process->pid, next_process->s_regs.rip);
next_process->k_stack[1023] = next_process->s_regs.ss;
next_process->k_stack[1022] = next_process->s_regs.rsp;
next_process->k_stack[1021] = next_process->s_regs.flags;
next_process->k_stack[1020] = next_process->s_regs.cs;
next_process->k_stack[1019] = next_process->s_regs.rip;
next_process->k_stack[1018] = next_process->s_regs.rax;
next_process->k_stack[1017] = next_process->s_regs.rbx;
next_process->k_stack[1016] = next_process->s_regs.rcx;
next_process->k_stack[1015] = next_process->s_regs.rdx;
next_process->k_stack[1014] = next_process->s_regs.rsi;
next_process->k_stack[1013] = next_process->s_regs.rdi;
next_process->k_stack[1012] = next_process->s_regs.rbp;
next_process->k_stack[1011] = next_process->s_regs.r8;
next_process->k_stack[1010] = next_process->s_regs.r9;
next_process->k_stack[1009] = next_process->s_regs.r10;
next_process->k_stack[1008] = next_process->s_regs.r11;
next_process->k_stack[1007] = next_process->s_regs.r12;
next_process->k_stack[1006] = next_process->s_regs.r13;
next_process->k_stack[1005] = next_process->s_regs.r14;
next_process->k_stack[1004] = next_process->s_regs.r15;

  write_cr3(next_process->cr3);
        current_process = next_process;
        tss.rsp0 = (uint64_t)&(next_process->k_stack[1023]);
        __asm__ __volatile__ ("movq %0, %%rsp" :: "r" (&(next_process->k_stack[1004])));
	   __asm__ __volatile__(
                                "popq %%r15\n\t"
                                "popq %%r14\n\t"
                                "popq %%r13\n\t"
                                "popq %%r12\n\t"
                                "popq %%r11\n\t"
                                "popq %%r10\n\t"
                                "popq %%r9\n\t"
                                "popq %%r8\n\t"
                                "popq %%rbp\n\t"
                                "popq %%rdi\n\t"
                                "popq %%rsi\n\t"
                                "popq %%rdx\n\t"
                                "popq %%rcx\n\t"
                                "popq %%rbx\n\t"
                                "popq %%rax\n\t" :::"memory");
	   __asm__ __volatile__(
                                "iretq"
                );
}

void schedule(struct regs_sched r) {
	check_sleeping_procs();
	 move_to_tail(&ready_queue);
//	printf("Printing List");
//	print_list(&ready_queue);
	if(current_process->pid == ready_queue->task->pid)
		return;
	current_process->s_regs = r;
	context_switch();
}

// current process sleeps for a specified number of seconds.
void k_sleep(uint64_t time, struct regs_sched r) {
	current_process->sleep_time = time;

	// This handling is not needed for preemptive system. Keep it to be safe
	if(time == 0)
		move_to_tail(&ready_queue);
	else{
		delete_in_pcb_list(&ready_queue, current_process);
		insert_to_tail_pcb_list(&wait_queue, current_process);
	}
	current_process->s_regs = r;	
	context_switch();
}
// Searches only from ready or wait queue
pcb *get_process_by_pid(int pid){
	// TODO
/*	if(pid < 2)
		return NULL;*/
	struct pcb_list * list = ready_queue;
	while (list) {
		if (list->task->pid == pid) {
			return list->task;
		}
		list = list->next;
	}

	list = wait_queue;
	while (list) {
		if (list->task->pid == pid) {
			return list->task;
		}
		list = list->next;
	}
		
     return NULL;
}

void assign_foreground(){
        if(current_process->pid != fg_process->pid)
                return;
        pcb *proc = get_process_by_pid(current_process->ppid);
        if(proc == NULL){
              printf("There is no foreground process left\n");
              while(1);
        }
	fg_process = proc;
}

void exit_process() {
//	printf("\nExiting pid: %d\n", current_process->pid);
        // should we take care of zombie processes? TODO nitish
        //traverse wait queue to check if any process is waiting on the current process. If yes move it to ready.
        struct pcb_list * list = wait_queue;
	uint16_t zombie = 1;
//        if (list == NULL)
//                printf("In exit method: Waiting queue is empty\n");
        while (list != NULL) {
                if (list->task->is_waiting_for == current_process->pid) {
                        pcb *temp = list->task;
                        delete_in_pcb_list(&wait_queue, temp);
                        insert_to_tail_pcb_list(&ready_queue, temp);
			zombie = 0; // Some parent was waiting for this process
                        break;
                }
                list = list->next;
        }
        delete_in_pcb_list(&ready_queue, current_process);
	if(zombie == 1){
        	insert_to_tail_pcb_list(&terminated_queue, current_process);
	}
	else{
	//	free_proc(current_process);
		kfree1(current_process);
	}
        delete_page_tables(current_process);  
	assign_foreground();
	claim_pid(current_process->pid);
	context_switch();
}

//options parameter and status not handled //TODO nitish
int k_waitpid(int pid, struct regs_sched r) {
//	printf("proc %d waiting for pid: %d", current_process->pid, pid);
	// if pid is not the child of current process // Doing it along with searching for pid in the code below

	//if wait pid is less than 0 wait for any child
	if (pid <= 0) {
		//wait for any child process
		// look for child in ready queue
		struct pcb_list *list_ready = ready_queue;
//		if (list_ready == NULL)
//			printf("In Wait_pid method: Ready_queue is empty");
		while (list_ready) {
			if (list_ready->task->ppid == current_process->pid) {
				current_process->is_waiting_for = pid;
				insert_to_tail_pcb_list(&wait_queue, current_process); //wrong nitish instead keep some wait variable in pcb
				delete_in_pcb_list(&ready_queue, current_process);
				current_process->s_regs = r;
				current_process->s_regs.rax = pid; 
				context_switch();
				break;
			}
			list_ready = list_ready->next;
		}
		if (list_ready != NULL)
			return list_ready->task->pid;  // return the child pid

		//else do nothing move forward and look in wait queue

		struct pcb_list * list_wait = wait_queue;
//		if (list_wait == NULL)
//			printf("In Wait_pid method: Wait_queue is empty");
		while (list_wait) {
			if (list_wait->task->ppid == current_process->pid) {
				current_process->is_waiting_for = pid;
				insert_to_tail_pcb_list(&wait_queue, current_process); //wrong nitish instead keep some wait variable in pcb
				delete_in_pcb_list(&ready_queue, current_process);
				current_process->s_regs = r; 
				current_process->s_regs.rax = pid; 
                                context_switch();	
				break;
			}
			list_wait = list_wait->next;
		}
		if (list_wait != NULL) // If child process found in the ready list return -1
			return list_wait->task->pid;  // return the child pid
		else
		{
//			printf("WaitPid: No child process for this ");
			return -MYECHILD;
		}

	} else {
		//check if pid is already terminated process
		struct pcb_list * list = terminated_queue;
		if (list == NULL);
//			printf("Waitpid: The process to be waited is not terminated yet!");
		while (list) {
			if (list->task->pid == pid && list->task->ppid == current_process->pid) {    // Also checking if the found pid is child of current process
				delete_in_pcb_list(&terminated_queue, list->task);
				//	free_proc(list->task);
					kfree1(list->task);
				return pid;
			}
			list = list->next;
		}
		//If not in terminated state then look for child process in ready queue  //WHY NOT IN WAIT QUEUE CHECK TODO nitish
		struct pcb_list * list_ready = ready_queue;
//		if (list_ready == NULL)
//			printf("In Wait_pid method: Ready_queue is empty");
		while (list_ready) {
			//printf("Four values: %d, %d, %d, %d\n",list_ready->task->pid, pid, list_ready->task->ppid, current_process->pid);
			if (list_ready->task->pid == pid && list_ready->task->ppid == current_process->pid) {
				current_process->is_waiting_for = pid;
				insert_to_tail_pcb_list(&wait_queue, current_process); //wrong nitish instead keep some wait variable in pcb
				delete_in_pcb_list(&ready_queue, current_process);
				current_process->s_regs = r; 
				current_process->s_regs.rax = pid; 
                                context_switch();	
				break;
			}
			list_ready = list_ready->next;
		}

		if (list_ready != NULL) // If child process found in the ready list return -1
			return list_ready->task->pid;  // return the child pid

		//else do nothing move forward and look in wait queue

		struct pcb_list * list_wait = wait_queue;
//		if (list_wait == NULL)
//			printf("In Wait_pid method: Wait_queue is empty");
		while (list_wait->next) {
			if (list_wait->task->pid == pid && list_wait->task->ppid == current_process->pid) {
				current_process->is_waiting_for = pid;
				insert_to_tail_pcb_list(&wait_queue, current_process); //wrong nitish instead keep some wait variable in pcb
				delete_in_pcb_list(&ready_queue, current_process);
				current_process->s_regs = r; 
				current_process->s_regs.rax = pid; 
                                context_switch();	
				break;
			}
			list_wait = list_wait->next;
		}
		if (list_wait->next != NULL)
			return list_wait->task->pid;  // return the child pid
		else return -MYECHILD;
	}
}

int k_execve(char *filename, char **argv, char **env) {
 char args[20][100]; // array of strings for argument variables to execve
 char env_p[20][100];
 int argc=0;
 int env_c=0; 
	int len = strlen(filename);
        char *file = (char *)kmalloc1(strlen(filename));
	memset(file, 0, len);
        memcpy(file, filename, len); 
	file[strlen(filename)] = '\0';
	//printf("Size of the filename: %s %d\n", filename, strlen(filename));
//	printf("In execve filename: %s\n", filename);

	if(file[0] == '/'){
		file += 8;
	//	file[strlen(file)] = '\0';
		strcpy(args[0], file);
		argc=1;
	}
//	printf("args[0] %s \t", args[0]);
//	printf("argv[0] %s \t", argv[0]);
//	printf("argv[1] %s \t", argv[1]);

    if (argv!=NULL) { 
        while (argv[argc]!=NULL) { 
            strcpy(args[argc], argv[argc]); 
            argc++; 
        } 
    }
//printf("1\t");
 if (env!=NULL) {
        while (env[env_c]!=NULL) {
            strcpy(env_p[env_c], env[env_c]);
            env_c++;
        }
    }
//printf("2\t");
        pcb *new_process = (pcb *)kmalloc1(sizeof(pcb));
    if (!new_process) {
//        printf("k_excve: NO space for new pcb");
       return -MYENOMEM; 
    }
        memset(new_process, 0, sizeof(pcb));
        strcpy(new_process->cur_working_dir, "/rootfs/");
        init_file_desc(new_process);
    int ret = init_virtual_space_proc(new_process);
    if (!(ret)) {
        write_cr3(new_process->cr3);
    } else{
//        printf("Unable to initialize process address space\n");
    	return -MYENOMEM;
    }

    if(load_process_tarfs(new_process, file) == -1){
//	    printf("Load tarfs returning -1\n");
	    write_cr3(current_process->cr3);
	    return -MYENOENT;
    }
//	kfree1(file);
    init_heap(new_process);
    init_stack(new_process); // Giving fixed size of 8 MB to the stack
  
/********set argument variables on top of user stack*******/
        
        uint64_t *temp_stack = (uint64_t *)new_process->u_stack;
//printf("--DONE WITH LOAD TARFS--");
//Store the environment variables
    for (int i = env_c-1; i >= 0; i--) {
        temp_stack--;
        *temp_stack =(uint64_t) env_p[i];
        }
    temp_stack--;   

 // Store the argument values
    for (int i = argc-1; i >= 0; i--) {
        temp_stack--;
        *temp_stack =(uint64_t) args[i];
        }
    temp_stack--;
    *temp_stack = (uint64_t)argc;
    new_process->u_stack = (uint64_t *) temp_stack;

    new_process->alarm_time=0;
    new_process->pid = current_process->pid;
    //new_process->pid = generate_pid();
    new_process->ppid = current_process->ppid;
    struct regs_sched s = {.r15 = 0,.r14 = 0,.r13 = 0,.r12 = 0,.r11 = 0,.r10 = 0,.r9 = 0,.r8 = 0,.rbp = 0,.rdi = 0,.rsi = 0,.rdx = 0,.rcx=0, .rbx=0, .rax=0,.rip=0,.rsp=0}; 
    new_process->s_regs = s;
    new_process->s_regs.rip = new_process->rip;
    new_process->s_regs.rsp = (uint64_t) new_process->u_stack;
    new_process->s_regs.ss = 0x23;
    new_process->s_regs.cs = 0x1B;
    new_process->s_regs.flags = 0x200286;
    new_process->k_stack[1023] = 0x23;
    new_process->k_stack[1022] = (uint64_t) new_process->u_stack;
    new_process->k_stack[1021] = 0x200286;
    new_process->k_stack[1020] = 0x1B;
    new_process->k_stack[1019] = new_process->rip;
    delete_in_pcb_list(&ready_queue, current_process);
    insert_to_head_pcb_list(&ready_queue, new_process);


   copy_tarfs_stuff(new_process, current_process);
  
  delete_page_tables(current_process);
    write_cr3(new_process->cr3);

    if(fg_process && fg_process->pid == current_process->pid)
		fg_process = new_process;
   context_switch();
    return -MYEACCES; // Control will never reach here.
}
