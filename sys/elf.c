#include <sys/tarfs.h>
#include <sys/sbunix.h>

// search the file into tarfs dir, return the elf pointer
char* search_tarfs(char *filename) {
    
    struct posix_header_ustar *ptr;
    char *iter = &_binary_tarfs_start;
    uint64_t size_posix_header = sizeof(struct posix_header_ustar);
    uint64_t size_file;
    while (iter < &_binary_tarfs_end) {
        ptr = (struct posix_header_ustar *) iter;
        size_file = oct_to_dec(atoi(ptr->size));
        if (strcmp(ptr->name, filename) == 0)
	{
            return (char *)(iter+size_posix_header);
	    
        }
	if (size_file % size_posix_header)
            size_file = ((size_file/size_posix_header)+1) * size_posix_header;
	iter += size_posix_header + size_file;
    }
    return NULL;
}

void map_segments_to_mem(Elf64_Phdr *header_table, Elf64_Ehdr *elf_ptr){
    uint64_t i;
    uint64_t *segment_content = (uint64_t *)((uint64_t)elf_ptr + header_table->p_offset);
    
    uint64_t *va = (uint64_t *)header_table->p_vaddr;
   for(i=0;i < header_table->p_filesz; i+=8){
        *(va++) = *(segment_content++);
    }
    for(;i<header_table->p_memsz;i+=8)
        *(va++) = 0;
}

int load_process_tarfs(pcb *proc, char * filename) {

    // Got the file location.
    char *elf_addr= search_tarfs(filename);
    if(elf_addr == NULL)
	return -1;
    Elf64_Ehdr* elf_ptr = (struct Elf64_Ehdr*)(elf_addr);
    proc->rip = elf_ptr->e_entry;
    Elf64_Phdr* header_table= (Elf64_Phdr *)((uint64_t)(elf_addr) + elf_ptr->e_phoff);
    uint16_t n_segments = elf_ptr->e_phnum;
    uint16_t size_ht_entry= elf_ptr->e_phentsize;
    proc->mm_st = (struct mm_struct *)kmalloc1(sizeof(struct mm_struct));
    memset(proc->mm_st, 0, sizeof(pcb)); 
	int k = 0;
    for(uint16_t i = 0; i < n_segments; i++ )
    {
	if(header_table->p_type == 1){
		//continue;
	k++;     
   	struct vm_area_struct *proc_vma = (struct vm_area_struct *)kmalloc1(sizeof(struct vm_area_struct));
        proc_vma->start_add = header_table->p_vaddr;
        proc_vma->end_add = header_table->p_vaddr + header_table->p_memsz;
        proc_vma->vma_flags = header_table->p_flags;
        proc_vma->vma_next = NULL;
        proc_vma->vma_prev = NULL;
        proc->brk_ptr = (proc_vma->end_add & 0xFFFFFFFFFFFFF000) + 4096;
        // Adding vma to the mmapped list. Add to the end if the list has already been initiated
        if(proc->mm_st->mmap == NULL)
            proc->mm_st->mmap = proc_vma;
        else {
            // Adding to the tail.
            struct vm_area_struct *curr = proc->mm_st->mmap;
            while(curr->vma_next != NULL)
                curr = curr->vma_next;
            curr->vma_next = proc_vma;
            proc_vma->vma_prev = curr;
            
        }
        map_segments_to_mem(header_table, elf_ptr);
        header_table = (Elf64_Phdr *)((uint64_t)header_table + size_ht_entry);
    
	}
    }
   return 0;
}
