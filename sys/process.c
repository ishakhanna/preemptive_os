#include "sys/sbunix.h"
#include <sys/gdt.h>
volatile uint16_t pid_bitmap[MAX_PID]={0};
uint64_t PID=1;
uint16_t PID_BASE=0;

extern struct pcb_list *ready_queue;
extern struct pcb_list *wait_queue;
extern pml4 *table_base_ptr;
pcb *current_process = NULL;
pcb *fg_process = NULL;
pcb *idle_process = NULL;
/* Murali changes */
void init_file_desc(pcb * process)
{
	int i = 0;
	while(i < MAX_OPEN_FILES)
	{
		process->file_descriptor[i] = -1;
		i++;
	}
}

void claim_pid(int pid){
	pid_bitmap[pid-1]=0;
//	uint16_t p = pid - 1;
//	pid_bitmap[p/8] &= ~(1 << (p % 8));
}

uint64_t generate_pid()
{
	uint16_t i = 0;
	if(PID_BASE > (MAX_PID-1))
	    PID_BASE = 0;
	PID_BASE++;
	for(i = PID_BASE; i < MAX_PID; i++){
		if(pid_bitmap[i]==0){
			pid_bitmap[i]=1;
			return i;
		}	
	}
//	printf("In generate_pid: No more PID available");
	return 0; // return 0 if no more process	
}

pcb *init_pcb(int idle)
{
        uint16_t new_pid;
	if(idle == 1)
		new_pid = MAX_PID+1;
	else new_pid = generate_pid();
	pcb *proc = NULL;
	if(new_pid == 0)
		return NULL;

	proc= (pcb *)kmalloc1(sizeof(pcb));
	memset(proc, 0, sizeof(pcb));
       	proc->sleep_time=0;
       	proc->ppid=0;    // for this process, parent is kernel
 	proc->pid= new_pid;
       	proc->is_waiting_for=0;
	strcpy(proc->cur_working_dir, "/rootfs/");
	init_file_desc(proc);
	proc->alarm_time=0;
	
  	return proc;
}

void print_vma(pcb *proc){
    if(proc->mm_st->mmap == NULL){
     	printf("NULL");
	   return;
    }
    else {
        struct vm_area_struct *curr = proc->mm_st->mmap;
	printf("Printing VMA details as: start_add end_add next prev current_vma_add\n");
        while(curr != NULL){
	printf("vma: %p %p %p %p %p\n", curr->start_add, curr->end_add, curr->vma_next, curr->vma_prev, (uint64_t)curr);	
	curr = curr->vma_next;
       }
    }

}
void copy_vma(pcb *child, pcb *parent){
    if(parent->mm_st->mmap == NULL)
        return;
    else {
        struct vm_area_struct *curr = parent->mm_st->mmap;
        struct vm_area_struct *child_prev = parent->mm_st->mmap;
        struct vm_area_struct *child_curr;
        while(curr != NULL){
            if(child->mm_st->mmap == NULL){
                child->mm_st->mmap = (struct vm_area_struct *)kmalloc1(sizeof(struct vm_area_struct));
                memcpy(child->mm_st->mmap, curr, sizeof(struct vm_area_struct));
                child_prev = child->mm_st->mmap;
                child_prev->vma_next = child_prev->vma_prev = NULL;
            }
            else{
                child_curr = (struct vm_area_struct *)kmalloc1(sizeof(struct vm_area_struct));
                memcpy(child_curr, curr, sizeof(struct vm_area_struct));
                child_curr->vma_next = NULL;
                child_curr->vma_prev = child_prev;
                child_prev->vma_next = child_curr;
                child_prev = child_curr;
            }
            curr = curr->vma_next;
        }
    }
}


void init_idle_process(){
	// 1 signifies that this is idle process
    pcb *proc = init_pcb(1);
    int ret = init_virtual_space_proc(proc);
    
    if(!(ret)){
        write_cr3(proc->cr3);
    }
    else{
        printf("Unable to initialize process address space\n");
        while(1);
    }
    
    load_process_tarfs(proc, "bin/idle");
    init_heap(proc);
    init_stack(proc);
    // Setting the kernel stack for proc
    memset(proc->k_stack, 0 ,1024*8);
    struct regs_sched s = {.r15 = 0,.r14 = 0,.r13 = 0,.r12 = 0,.r11 = 0,.r10 = 0,.r9 = 0,.r8 = 0,.rbp = 0,.rdi = 0,.rsi = 0,.rdx = 0,.rcx=0, .rbx=0, .rax=0,.rip=0,.rsp=0};
    proc->s_regs = s;
    proc->s_regs.rip = proc->rip;
    proc->s_regs.rsp = (uint64_t)proc->u_stack;
    proc->s_regs.ss = 0x23;
    proc->s_regs.cs = 0x1B;
    proc->s_regs.flags = 0x200286;
    write_cr3((uint64_t)table_base_ptr);
    idle_process = proc; 
}

int print_processes(){
	printf("\nPID\t TTY\n");
	struct pcb_list *curr = ready_queue;
        while(curr != NULL){
                printf("%d\t ttys001\n", curr->task->pid);
                curr = curr->next;
        }
	curr = wait_queue;
        while(curr != NULL){
                printf("%d\t ttys001\n", curr->task->pid);
                curr = curr->next;
        }
	return 0;
}

int kill_process(int pid){
//	printf("\n--------KILLING PROCESS WITH PID : %d -------------------\n", pid);
//	printf("LISTS BEFORE KILL\n");
//	print_list(&ready_queue);
//	print_list(&wait_queue);
//	print_list(&terminated_queue);

	pcb *proc = get_process_by_pid(pid);	
	if(proc == NULL)
		return -MYESRCH;
	uint16_t zombie = 1;
        struct pcb_list * list = wait_queue;
//        if (list == NULL)
//                printf("In exit method: Waiting queue is empty\n");
        while (list != NULL) {
                if (list->task->is_waiting_for == pid) {
                        pcb *temp = list->task;
                        delete_in_pcb_list(&wait_queue, temp);
                        insert_to_tail_pcb_list(&ready_queue, temp);
			zombie = 0; // Some parent was waiting for this process
                        break;
                }
                list = list->next;
        }
        // delete the current process from ready queue and add to zombie
        delete_in_pcb_list(&ready_queue, proc);
	if(zombie == 1){
        	insert_to_tail_pcb_list(&terminated_queue, proc);
	}
	else{
		//free_proc(proc);
		kfree1(proc);
	}
        delete_page_tables(proc);
    	write_cr3(current_process->cr3);
        claim_pid(proc->pid);
	if(current_process->pid == pid){
		assign_foreground();
//		kfree1(proc);
		context_switch();
	}
//	printf("\nLISTS AFTER KILL\n");
//	print_list(&ready_queue);
//	print_list(&wait_queue);
//	print_list(&terminated_queue);
//	kfree1(proc);
	return 0;
}

void init_proc2(){
	pcb *proc = init_pcb(0);

	char *env_p[] = {"PATH=/rootfs/bin","CWD=/rootfs",NULL};
        int env_c=2;
        char *arg_v[] = {"bin/hello2",NULL};
        uint64_t arg_c=1;  	

 	int ret = init_virtual_space_proc(proc);
 
	if(!(ret)){
   		write_cr3(proc->cr3);	
	}
    else{
        printf("Unable to initialize process address space\n");
        while(1);
    }
  	
       load_process_tarfs(proc, "bin/hello2");
        init_heap(proc);
        init_stack(proc);
	memset((uint64_t *)((uint64_t)proc->u_stack - 8192),0,8192);
	
	/*----------------------------------------------------------*/

	uint64_t *temp_stack = (uint64_t *)proc->u_stack;

	temp_stack--;
	*temp_stack = ((uint64_t)0);
	//Store the environment variables
	for (int i = env_c-1; i >= 0; i--) {
        	temp_stack--;
        	*temp_stack =(uint64_t) env_p[i];
        }
	temp_stack--;
	*temp_stack = ((uint64_t)0);

  //Store the argument values
    for (int i = arg_c-1; i >= 0; i--) {
        temp_stack--;
        *temp_stack =(uint64_t) arg_v[i];
        }
    temp_stack--;

    *temp_stack = (uint64_t)arg_c;

    proc->u_stack = (uint64_t *)temp_stack;
	
	/*----------------------------------------------------------*/
	
	 // Setting the kernel stack for proc
    	memset(proc->k_stack, 0 ,1024*8); 
	struct regs_sched s = {.r15 = 0,.r14 = 0,.r13 = 0,.r12 = 0,.r11 = 0,.r10 = 0,.r9 = 0,.r8 = 0,.rbp = 0,.rdi = 0,.rsi = 0,.rdx = 0,.rcx=0, .rbx=0, .rax=0,.rip=0,.rsp=0};
	proc->s_regs = s;
	proc->s_regs.rip = proc->rip;
        proc->s_regs.rsp = (uint64_t)proc->u_stack;
	proc->s_regs.ss = 0x23;
	proc->s_regs.cs = 0x1B;
	proc->s_regs.flags = 0x200286;
	proc->k_stack[1023] = 0x23;
        proc->k_stack[1022] = (uint64_t)proc->u_stack;
        proc->k_stack[1021] = 0x200286;
        proc->k_stack[1020] = 0x1B;
        proc->k_stack[1019] = proc->rip;	
//	proc->rsp = &(proc->k_stack[KSTACK_INITIAL_PTR]);
	insert_to_head_pcb_list(&ready_queue,proc);

	write_cr3((uint64_t)table_base_ptr);        
}

void free_proc(pcb *proc){
        if(proc == NULL || proc->mm_st == NULL || proc->mm_st->mmap == NULL)
                return;
        struct vm_area_struct *curr = proc->mm_st->mmap;
        while(curr != NULL){
                struct vm_area_struct *temp = curr;
                curr = curr->vma_next;
                kfree1(temp);
        }
}

void init_process(){
/*---------------------------INITIALIZING PROC1----------------------------*/

	pcb *proc = init_pcb(0);

	char *env_p[] = {"PATH=/rootfs/bin","CWD=/rootfs",NULL};
        int env_c=2;
        //char *arg_v[] = {"bin/sbush",NULL};
        char *arg_v[] = {shell_bin,NULL};
        uint64_t arg_c=1;  	

 	int ret = init_virtual_space_proc(proc);
 
	if(!(ret)){
   		write_cr3(proc->cr3);	
	}
    else{
        printf("Unable to initialize process address space\n");
        while(1);
    }

  	//load_process_tarfs(proc, "bin/sbush");
       load_process_tarfs(proc, shell_bin);
        init_heap(proc);
        init_stack(proc);
	memset((uint64_t *)((uint64_t)proc->u_stack - 8192),0,8192);
	
	/*----------------------------------------------------------*/

	uint64_t *temp_stack = (uint64_t *)proc->u_stack;

	temp_stack--;
	*temp_stack = ((uint64_t)0);
	//Store the environment variables
	for (int i = env_c-1; i >= 0; i--) {
        	temp_stack--;
        	*temp_stack =(uint64_t) env_p[i];
        }
	temp_stack--;
	*temp_stack = ((uint64_t)0);

  //Store the argument values
    for (int i = arg_c-1; i >= 0; i--) {
        temp_stack--;
        *temp_stack =(uint64_t) arg_v[i];
        }
    temp_stack--;

    *temp_stack = (uint64_t)arg_c;

    proc->u_stack = (uint64_t *)temp_stack;
	
	/*----------------------------------------------------------*/
	
	 // Setting the kernel stack for proc
    	memset(proc->k_stack, 0 ,1024*8); 
	struct regs_sched s = {.r15 = 0,.r14 = 0,.r13 = 0,.r12 = 0,.r11 = 0,.r10 = 0,.r9 = 0,.r8 = 0,.rbp = 0,.rdi = 0,.rsi = 0,.rdx = 0,.rcx=0, .rbx=0, .rax=0,.rip=0,.rsp=0};
	proc->s_regs = s;
	proc->s_regs.rip = proc->rip;
        proc->s_regs.rsp = (uint64_t)proc->u_stack;
	proc->s_regs.ss = 0x23;
	proc->s_regs.cs = 0x1B;
	proc->s_regs.flags = 0x200286;
	proc->k_stack[1023] = 0x23;
        proc->k_stack[1022] = (uint64_t)proc->u_stack;
        proc->k_stack[1021] = 0x200286;
        proc->k_stack[1020] = 0x1B;
        proc->k_stack[1019] = proc->rip;	
//	proc->rsp = &(proc->k_stack[KSTACK_INITIAL_PTR]);
	insert_to_head_pcb_list(&ready_queue,proc);

	write_cr3((uint64_t)table_base_ptr);        

	init_idle_process();
	//init_proc2();

/*--------- RING 3 SWITCH------------------*/	
	write_cr3(proc->cr3);
	
    	tss.rsp0 = (uint64_t)&(proc->k_stack[1023]); // 1023 because stack grows downward. So rsp0 hsould point to highest address.
  	current_process = proc;       
  	fg_process = proc;       
	__asm__ __volatile__("movq %0, %%rbx\n\t"
			     "movq %1, %%rbp\n\t"::"r"((uint64_t)0), "r"((uint64_t)0):"rbx","rbp"		
	);
	uint64_t tem = 0x28;
	__asm__ __volatile__("cli");
	__asm__ __volatile__("mov %0,%%rax;"::"r"(tem));
	__asm__ __volatile__("ltr %ax");
	
	__asm__ __volatile__("\
	push $0x23;\
	push %0;\
	pushf;\
	pop %%rax;\
	or $0x200, %%rax;\
	push %%rax;\
	push $0x1B;\
	push %1"::"g"(proc->u_stack),"g"(proc->rip):"memory");
	
//	printf("SWITCHING TO RING 3\n");
//    printf("In kernel stack top=%p address=%p\n",*proc->u_stack,proc->u_stack);

	__asm__ __volatile__("\
	iretq;\
  ");
}

void init_heap(pcb *proc){
    struct vm_area_struct *heap_vma = (struct vm_area_struct *)kmalloc1(sizeof(struct vm_area_struct));
    
    heap_vma->end_add = heap_vma->start_add = proc->brk_ptr; // Initially heap is of size 0
    heap_vma->vma_next = NULL;
    heap_vma->vma_prev = NULL;
    heap_vma->vma_flags = VMA_W + VMA_R;
    
    if(proc->mm_st->mmap == NULL)
        proc->mm_st->mmap = heap_vma;
    else {
        // Adding to the tail.
        struct vm_area_struct *curr = proc->mm_st->mmap;
        while(curr->vma_next != NULL)
            curr = curr->vma_next;
        curr->vma_next = heap_vma;
        heap_vma->vma_prev = curr;
    }
}

struct vm_area_struct *find_vma(struct vm_area_struct *mmap, uint64_t addr){
    if(mmap == NULL)
        return NULL;
    struct vm_area_struct *curr = mmap;
    while(curr != NULL && (!(curr->start_add <= addr && addr <= curr->end_add)))
	curr = curr->vma_next;
	 return curr;
}

uint64_t extend_heap(pcb *proc, uint64_t new_brk_ptr){
    uint64_t old_brk_ptr = proc->brk_ptr;
    if(new_brk_ptr >= ((uint64_t)proc->u_stack)-(uint64_t)STACK_SIZE){
//        printf("HEAP OVERFLOWED INTO STACK --- KILL PROCESS\n");
	    return 0;
    }
    struct vm_area_struct *heap_vma = find_vma(proc->mm_st->mmap, old_brk_ptr);
    heap_vma->end_add = new_brk_ptr;
    proc->brk_ptr = new_brk_ptr;
    return new_brk_ptr;
}

uint64_t get_stack_end(pcb *proc){
	struct vm_area_struct *stack_vma = find_vma(proc->mm_st->mmap, STACK_START);
	return stack_vma->start_add;
}

int extend_stack(pcb *proc, uint64_t new_top){
	if(new_top < (STACK_START - MAX_STACK_SIZE))
		return -1;
	 struct vm_area_struct *stack_vma = find_vma(proc->mm_st->mmap, STACK_START);
	 stack_vma->start_add -= PAGE_SIZE;		
	return 0;
}

void init_stack(pcb *proc){
    proc->u_stack = (uint64_t *)kmalloc1(STACK_SIZE);
   // uint64_t stack_start = (uint64_t)VIRTUAL_OFFSET - 4096;
    uint64_t stack_start = STACK_START;
    uint64_t bump_ptr = stack_start - (size_t)STACK_SIZE;
    proc->u_stack = (uint64_t *)stack_start;

    for(uint64_t i = stack_start; i >= bump_ptr; i -= PAGE_SIZE)
	assign_page_with_self_ref(i);	
struct vm_area_struct *stack_vma = (struct vm_area_struct *)kmalloc1(sizeof(struct vm_area_struct));

    stack_vma->end_add = stack_start;
    stack_vma->start_add = bump_ptr;
    stack_vma->vma_next = NULL;
    stack_vma->vma_prev = NULL;
    stack_vma->vma_flags = VMA_W + VMA_R;
    
    if(proc->mm_st->mmap == NULL)
        proc->mm_st->mmap = stack_vma;
    else {
        // Adding to the tail.
        struct vm_area_struct *curr = proc->mm_st->mmap;
        while(curr->vma_next != NULL)
            curr = curr->vma_next;
        curr->vma_next = stack_vma;
        stack_vma->vma_prev = curr;
    }
}
                        
int copy_virtual_space(pcb *child, pcb *parent){
    write_cr3((uint64_t)table_base_ptr);
    uint64_t pml4_child_phy_add = (uint64_t)get_free_page();
    
    if(!pml4_child_phy_add){
        return -1;
    }
    pml4 *pml4_child_virt_add, *pml4_parent_virt_add, *pml4_kernel_add;
    
    // Get kernel pml4 physical address using self referencing trick.
    pml4_kernel_add = (pml4 *)get_self_ref_add(0x1FE,0x1FE,0x1FE,0x1FE);
    int i = 509;
    // Find first available slot in kernel pml4 to save the process pml4 address
//    for(i = 509; i>=0; i--){
//        if(pml4_kernel_add->entries[i] == 0x0){
            pml4_kernel_add->entries[i] = (((uint64_t) pml4_child_phy_add) | PR | RW | US);
//            break;
//        }
//    }
    
    // Get Child virtual address
    pml4_child_virt_add = (pml4 *)(get_self_ref_add(0x1FE, 0x1FE, 0x1FE, i));
    memset(pml4_child_virt_add,0,sizeof(pml4));
//    child->kernel_pml4_offset = i;

    // Map kernel entries to child address space.
    pml4_child_virt_add->entries[511] = pml4_kernel_add->entries[511];
    // Self Referencing
    pml4_child_virt_add->entries[0x1FE] = (uint64_t)pml4_child_phy_add | PR | RW | US;
    child->cr3 = (uint64_t)pml4_child_phy_add;

    write_cr3(parent->cr3);
    // Get parents virtual address
    pml4_parent_virt_add = (pml4 *)(get_self_ref_add(0x1FE, 0x1FE, 0x1FE, 0x1FE));
    pml4_parent_virt_add->entries[509] = (((uint64_t) pml4_child_phy_add) | PR | RW | US);

    for(int j = 509; j >=0; j--){
        if(pml4_parent_virt_add->entries[j] & (uint64_t)PR){
            pdp *pdpe_child_phy_add = (pdp *)get_free_page();
            if(!pdpe_child_phy_add){
                return -1;
            }
            pml4_child_virt_add->entries[j] = (((uint64_t)pdpe_child_phy_add) & 0xFFFFFFFFFF000) | PR | RW | US;
            
            pdp *pdpe_child_virt_add = (pdp *)get_self_ref_add(0x1FE,0x1FE,i,j);
            memset(pdpe_child_virt_add,0,sizeof(pdp));
            pdp *pdpe_parent_virt_add = (pdp *)get_self_ref_add(0x1FE,0x1FE,0x1FE,j);
            
            // Loop for next table level i.e. pdpe
            for(int k = 511; k >= 0; k--){
                if(pdpe_parent_virt_add->entries[k] & PR){
                    pd *pde_child_phy_add = (pd *)get_free_page();
            
                    if(!pde_child_phy_add){
                        return -1;
                    }
                    
                    pdpe_child_virt_add->entries[k] = (((uint64_t)pde_child_phy_add) & 0xFFFFFFFFFF000) | PR | RW | US;
                    
                    pd *pde_child_virt_add = (pd *)get_self_ref_add(0x1FE,i,j,k);
                    memset(pde_child_virt_add,0,sizeof(pd));
                    pd *pde_parent_virt_add = (pd *)get_self_ref_add(0x1FE,0x1FE,j,k);
                    
                    // Loop for next table level i.e. pd
                    for(int m = 511; m >= 0; m--){
                        if(pde_parent_virt_add->entries[m] & PR){
                            pt *pte_child_phy_add = (pt *)get_free_page();
                    
                            if(!pte_child_phy_add){
                                return -1;
                            }
                            
                //            memset(pte_child_phy_add,0,sizeof(pt));
                            
                            pde_child_virt_add->entries[m] = (((uint64_t)pte_child_phy_add) & 0xFFFFFFFFFF000) | PR | RW | US;
                            
                            pt *pte_child_virt_add = (pt *)get_self_ref_add(i,j,k, m);
                            memset(pte_child_virt_add,0,sizeof(pt));
                            pt *pte_parent_virt_add = (pt *)get_self_ref_add(0x1FE,j,k,m);
                            
                            // Loop the innermost pte to set the actual values and mark them as COW
                            for(int n = 511; n >= 0; n--){
                                if(pte_parent_virt_add->entries[n] & PR){
                                    if(pte_parent_virt_add->entries[n] & RW){
                                        // Mark the pages as read only
                                        pte_parent_virt_add->entries[n] = pte_parent_virt_add->entries[n] & 0xFFFFFFFFFFFFFFFD;
                                        // Mark COW bit
                                        pte_parent_virt_add->entries[n] = pte_parent_virt_add->entries[n] | COW_MASK;
                                    }
				    uint64_t frame_phys_addr = pte_parent_virt_add->entries[n] & 0xFFFFFFFFFF000;
				    incr_ref_count(frame_phys_addr);
                                    pte_child_virt_add->entries[n] = pte_parent_virt_add->entries[n];
                                }
                            }
                        }
                    }
                }
            }
        }
   }
    write_cr3(parent->cr3);
pml4_parent_virt_add->entries[509] = 0x0;
    return 0;
}

void copy_tarfs_stuff(pcb *child, pcb *parent){
 memcpy(child->file_descriptor, parent->file_descriptor, MAX_OPEN_FILES);
   strncpy(child->cur_working_dir, parent->cur_working_dir, PATH_MAX);
   memcpy(child->f_pos, parent->f_pos, MAX_OPEN_FILES);
   memcpy(child->mapping, parent->mapping, MAX_OPEN_FILES);
}
                         
uint64_t _fork(pcb *parent){
	pcb *child = init_pcb(0);
	if(child == NULL){
		return -MYENOMEM;
	}
/*-----------DEEP COPY-----------------*/
 child->mm_st = (struct mm_struct *)kmalloc1(sizeof(struct mm_struct));
memset(child->mm_st, 0, sizeof(pcb));
copy_vma(child, parent);
    int ret = 0;
     child->alarm_time=0;
    child->ppid = parent->pid;
//    memcpy(child->k_stack, parent->k_stack, 1024*8);
	child->s_regs = parent->s_regs;
    child->s_regs.rax = 0x0;
    insert_to_tail_pcb_list(&ready_queue,child);

/*----------------------U STACK COPY--------------------*/ 
uint64_t *temp_u_stack = (uint64_t *)kmalloc1(8192);
memcpy(temp_u_stack, (uint64_t *)((uint64_t)parent->u_stack - 8192), 8192);
  
    ret = copy_virtual_space(child,parent);
    if(!ret){
        write_cr3(child->cr3);
        child->u_stack = parent->u_stack;
        child->brk_ptr = parent->brk_ptr;

        memcpy((uint64_t *)((uint64_t)child->u_stack - 8192), temp_u_stack, 8192);
        copy_tarfs_stuff(child, parent);
        increment_file_desc(parent);
	}
    else{
    //	printf("Error in initializing address space\n");
    	return -MYENOMEM;
    }

    write_cr3(parent->cr3);
    parent->s_regs.rax = child->pid;    
    fg_process = child;
    return child->pid;
}

int get_free_fd(pcb *process)
{
	int i;
	for(i = 3; i < MAX_OPEN_FILES; i++)
	{
		if(process->file_descriptor[i] == -1)
		{
			return i;
		}
	}
	return 0;
}

uint64_t get_local_fd(pcb *process, uint64_t global_fd)
{

	// -777 is for rootfs directory
	if(global_fd < 0)
	{
		return global_fd;
	}

	int fd = get_free_fd(process);

	if(fd != 0)
	{
		process->file_descriptor[fd] = global_fd;
		process->mapping[fd] = 1;
		process->f_pos[fd] = 0;
		return fd;
	}
	else
	{
//		printf("Cannot open more than %d files.\n", MAX_OPEN_FILES - 1);
		return -1;
	}
}

int get_direct_local_fd(pcb * process, int fd)
{

	if(fd < 0)
	{
		return -1;
	}

	if(fd == 0 || fd == 1 || fd == 2)
	{
		if(process->mapping[fd] == 0) // not mapped to anything
		{
			return fd;
		}
	}

	if(process->mapping[fd] == 1)
	{
		return fd;
	}
	else if(process->mapping[fd] == 2)
	{
		return process->file_descriptor[fd];
	}
	return -1;

/*
	if(fd < 0)
	{
		return -1;
	}

	if(fd == 0 || fd == 1)
	{
		if(process->mapping[fd] == 0) // not mapped to anything
		{
			return fd;
		}
	}

	if(process->mapping[fd] == 1)
	{
		return fd;
	}
	else if(process->mapping[fd] == 2)
	{
		return process->file_descriptor[fd];
	}
	return 0;
*/
}

uint64_t get_global_fd(pcb *process, int local_fd)
{
	if(local_fd > 2 && local_fd < MAX_OPEN_FILES)
	{
		int global = process->file_descriptor[local_fd];
		if(global >= 0 || global == -77 || global == -88)
		{
			return global;
		}
	}
	return -9; //
}

void reset_local_fd(pcb * process, int local_fd)
{
	if(local_fd >= 0 && local_fd < MAX_OPEN_FILES)
	{
		/*Changes Thursday*/
		/*
		 * If the local_fd is direct mapping, then search the whole
		 * array and redirect them to corresponding values, before
		 * closing this main descriptor.
		 */
		int i = 0;
		while(i < MAX_OPEN_FILES)
		{
			if(process->file_descriptor[i] == local_fd && process->mapping[i] == 2)
			{
//				printf("Resolving indirect mapping for descriptor: %d\n", i);
				/* i th descriptor is relying on this local_fd through indirect mapping */
				process->file_descriptor[i] = process->file_descriptor[local_fd];
				process->mapping[i] = 1; // This become direct mapping now.
				process->f_pos[i] = process->f_pos[local_fd];
			}
			i++;
		}

		process->file_descriptor[local_fd] = -1;
		process->mapping[local_fd] = 0;
		process->f_pos[local_fd] = 0;
	}
}

char * get_current_working_directory(pcb * process)
{
	return process->cur_working_dir;
}

int get_length(char * str)
{
	int len = 0;
	while(str[len] != '\0')
	{
		len++;
	}
	return len;
}

int copy_current_directory(pcb * process, char * answer, int size)
{
	if(size  == 0 || answer == NULL)
	{
		return -22; //EINVAL
	}

	char * cwd = get_current_working_directory(process);
	int len = get_length(cwd);

	if(len > PATH_MAX)
	{
		return -36; //ENAMETOOLONG
	}

	if(len + 1 > size)
	{
		return -34; // ERANGE
	}

	strcpy(answer, cwd);
	answer[len] = '\0';

	return 0;
}

int duplicate_fd(pcb * process, int fd)
{
	int i = 3;

	while(i < MAX_OPEN_FILES && process->file_descriptor[i] != 0)
	{
		i++;
	}

	if(i >= MAX_OPEN_FILES)
	{
		return -1;
	}

	process->file_descriptor[i] = fd;
	process->f_pos[i] = 0; // not needed anyways!
	process->mapping[i] = 2; // indirect mapping
	return i; // duplicate descriptor
}

int duplicate_fd_2(pcb * process, int local_fd, int new_fd)
{
/*
	 Murali
	process->file_descriptor[new_fd] = process->file_descriptor[local_fd];
	process->mapping[new_fd] = process->file_descriptor[local_fd];

	return new_fd;
*/


	process->file_descriptor[new_fd] = local_fd;
	process->mapping[new_fd] = 2;

	return new_fd;

}

int not_mapped(pcb * process, int std)
{
	/*
	 * std can be stdin or stdout,
	 *
	 */
	if(process->mapping[std] == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
